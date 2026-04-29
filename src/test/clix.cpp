#include "nil/clix/node.hpp"
#include <nil/clix.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <iostream>

namespace
{
    auto help(const std::string& c)
    {
        return "OPTIONS:"
               "\n  -f [ --flag ]                         flag msg"
               "\n  -n [ --number ] [=value(=1)] (=0)     number msg"
               "\n  -p [ --param ] text (=\"default value\")"
               "\n                                        param msg"
               "\n  -m [ --mparam ] texts                 mparam msg"
               "\n"
               "\nSUBCOMMANDS:"
               "\n  sub"
            + c + "                                  description of sub" + c;
    }

    void apply(nil::clix::Node& node, testing::MockFunction<int(const nil::clix::Options&)>& mock)
    {
        flag(node, "flag", {.skey = 'f', .msg = "flag msg"});
        number(node, "number", {.skey = 'n', .msg = "number msg", .fallback = 0, .implicit = 1});
        param(node, "param", {.skey = 'p', .msg = "param msg", .fallback = "default value"});
        params(node, "mparam", {.skey = 'm', .msg = "mparam msg"});
        use(node, [&mock](const nil::clix::Options& options) { return mock.Call(options); });
    }
} // namespace

TEST(cli, base)
{
    auto args = std::array<const char*, 0>{};
    ASSERT_EQ(run(nil::clix::make_node(), args.size(), args.data()), 0);
}

TEST(cli, depth_one)
{
    testing::StrictMock<testing::MockFunction<int(const nil::clix::Options&)>> mock;

    const auto matches = [](const nil::clix::Options& output)
    {
        return !flag(output, "flag")                     //
            && number(output, "number") == 0             //
            && param(output, "param") == "default value" //
            && !has_value(output, "mparam");
    };
    EXPECT_CALL(mock, Call(testing::Truly(matches)))
        .Times(1)
        .WillOnce(testing::Return(0))
        .RetiresOnSaturation();

    auto args = std::array<const char*, 0>{};

    auto node = nil::clix::make_node();
    apply(node, mock);
    ASSERT_EQ(run(node, args.size(), args.data()), 0);
}

TEST(cli, depth_deep)
{
    testing::StrictMock<testing::MockFunction<int(const nil::clix::Options&)>> mock;

    auto root = nil::clix::make_node();

    apply(root, mock);

    sub(root,
        "sub1",
        "description of sub1",
        [&mock](nil::clix::Node& sub1)
        {
            apply(sub1, mock);
            sub(sub1,
                "sub2",
                "description of sub2",
                [&mock](nil::clix::Node& sub2)
                {
                    apply(sub2, mock);
                    sub(sub2,
                        "sub3",
                        "description of sub3",
                        [&mock](nil::clix::Node& sub3) { apply(sub3, mock); });
                });
        });

    const auto matches = [](std::string h)
    {
        return [h = std::move(h)](const nil::clix::Options& output)
        {
            std::ostringstream oss;
            help(output, oss);
            std::cout << __FILE__ << ':' << __LINE__ << ':' << (const char*)(__FUNCTION__)
                      << std::endl;
            std::cout << oss.str() << std::endl;
            std::cout << __FILE__ << ':' << __LINE__ << ':' << (const char*)(__FUNCTION__)
                      << std::endl;
            std::cout << help(h) << std::endl;
            std::cout << __FILE__ << ':' << __LINE__ << ':' << (const char*)(__FUNCTION__)
                      << std::endl;
            return oss.str() == help(h)                      //
                && !flag(output, "flag")                     //
                && number(output, "number") == 0             //
                && param(output, "param") == "default value" //
                && !has_value(output, "mparam");
        };
    };
    {
        EXPECT_CALL(mock, Call(testing::Truly(matches("1"))))
            .Times(1)
            .WillOnce(testing::Return(0))
            .RetiresOnSaturation();

        auto args = std::array<const char*, 0>{};
        ASSERT_EQ(run(root, args.size(), args.data()), 0);
    }
    {
        EXPECT_CALL(mock, Call(testing::Truly(matches("2"))))
            .Times(1)
            .WillOnce(testing::Return(0))
            .RetiresOnSaturation();

        auto args = std::array{"sub1"};
        ASSERT_EQ(run(root, args.size(), args.data()), 0);
    }
    {
        EXPECT_CALL(mock, Call(testing::Truly(matches("3"))))
            .Times(1)
            .WillOnce(testing::Return(0))
            .RetiresOnSaturation();

        auto args = std::array{"sub1", "sub2"};
        ASSERT_EQ(run(root, args.size(), args.data()), 0);
    }
}

TEST(cli, required_number_is_enforced)
{
    auto node = nil::clix::make_node();
    number(node, "thread", {.skey = 't', .msg = "number of threads"});
    use(node,
        [](const nil::clix::Options& options)
        {
            (void)number(options, "thread");
            return 0;
        });

    auto args = std::array<const char*, 0>{};

    EXPECT_THROW(run(node, args.size(), args.data()), std::invalid_argument);
}

TEST(cli, duplicate_long_option_is_rejected)
{
    auto node = nil::clix::make_node();

    flag(node, "help", {.skey = 'h', .msg = "show this help"});

    EXPECT_THROW(flag(node, "help", {.msg = "duplicate help"}), std::invalid_argument);
}

TEST(cli, duplicate_short_option_is_rejected)
{
    auto node = nil::clix::make_node();

    flag(node, "help", {.skey = 'h', .msg = "show this help"});

    EXPECT_THROW(
        number(node, "threads", {.skey = 'h', .msg = "thread count"}),
        std::invalid_argument
    );
}

TEST(cli, option_and_subcommand_can_share_name)
{
    testing::StrictMock<testing::MockFunction<int(const nil::clix::Options&)>> root_mock;
    testing::StrictMock<testing::MockFunction<void()>> sub_mock;

    auto node = nil::clix::make_node();

    flag(node, "help", {.skey = 'h', .msg = "show this help"});
    use(node, [&root_mock](const nil::clix::Options& options) { return root_mock.Call(options); });
    sub(node,
        "help",
        "help command",
        [&sub_mock](nil::clix::Node& subnode)
        {
            use(subnode,
                [&sub_mock](const nil::clix::Options&)
                {
                    sub_mock.Call();
                    return 17;
                });
        });

    EXPECT_CALL(
        root_mock,
        Call(testing::Truly([](const nil::clix::Options& options) { return flag(options, "help"); })
        )
    )
        .Times(1)
        .WillOnce(testing::Return(11));
    EXPECT_CALL(sub_mock, Call()).Times(1);

    auto option_args = std::array{"--help"};
    ASSERT_EQ(run(node, option_args.size(), option_args.data()), 11);

    auto subcommand_args = std::array{"help"};
    ASSERT_EQ(run(node, subcommand_args.size(), subcommand_args.data()), 17);
}

TEST(cli, access_reports_invalid_conversion)
{
    testing::StrictMock<testing::MockFunction<int(const nil::clix::Options&)>> mock;

    auto node = nil::clix::make_node();
    flag(node, "flag", {.skey = 'f', .msg = "flag msg"});
    use(node,
        [&mock](const nil::clix::Options& options)
        {
            mock.Call(options);
            return 0;
        });

    EXPECT_CALL(mock, Call(testing::_))
        .Times(1)
        .WillOnce(testing::Invoke(
            [](const nil::clix::Options& options)
            {
                EXPECT_THROW(number(options, "flag"), std::invalid_argument);
                return 0;
            }
        ));

    auto args = std::array{"--flag"};
    ASSERT_EQ(run(node, args.size(), args.data()), 0);
}

TEST(cli, access_reports_unknown_option)
{
    testing::StrictMock<testing::MockFunction<int(const nil::clix::Options&)>> mock;

    auto node = nil::clix::make_node();
    flag(node, "flag", {.skey = 'f', .msg = "flag msg"});
    use(node,
        [&mock](const nil::clix::Options& options)
        {
            mock.Call(options);
            return 0;
        });

    EXPECT_CALL(mock, Call(testing::_))
        .Times(1)
        .WillOnce(testing::Invoke(
            [](const nil::clix::Options& options)
            {
                EXPECT_THROW(flag(options, "missing"), std::out_of_range);
                return 0;
            }
        ));

    auto args = std::array<const char*, 0>{};
    ASSERT_EQ(run(node, args.size(), args.data()), 0);
}
