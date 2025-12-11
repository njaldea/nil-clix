#include "nil/clix/node.hpp"
#include <nil/clix.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

namespace
{
    auto help(const std::string& c)
    {
        return 
            "OPTIONS:"
            "\n  -f [ --flag ]                         flag msg"
            "\n  -n [ --number ] [=value(=1)] (=0)     number msg"
            "\n  -p [ --param ] text (=\"default value\")"
            "\n                                        param msg"
            "\n  -m [ --mparam ] text                  mparam msg"
            "\n"
            "\nSUBCOMMANDS:"
            "\n  sub" + c + "                                  description of sub" + c +
            "\n"
            "\n";
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
    auto args = std::array<const char*, 1>{"Program"};
    ASSERT_EQ(run(nil::clix::create_node(), args.size(), args.data()), 0);
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

    auto args = std::array{"Program"};

    auto node = nil::clix::create_node();
    apply(node, mock);
    ASSERT_EQ(run(node, args.size(), args.data()), 0);
}

TEST(cli, depth_deep)
{
    testing::StrictMock<testing::MockFunction<int(const nil::clix::Options&)>> mock;

    auto root = nil::clix::create_node();

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

    const auto matches = [](std::string cmd, std::string h)
    {
        return [c = std::move(cmd), h = std::move(h)](const nil::clix::Options& output)
        {
            std::ostringstream oss;
            help(output, oss);
            return oss.str() == help(h)                      //
                && !flag(output, "flag")                     //
                && number(output, "number") == 0             //
                && param(output, "param") == "default value" //
                && !has_value(output, "mparam");
        };
    };
    {
        EXPECT_CALL(mock, Call(testing::Truly(matches("Program", "1"))))
            .Times(1)
            .WillOnce(testing::Return(0))
            .RetiresOnSaturation();

        auto args = std::array{"Program"};
        ASSERT_EQ(run(root, args.size(), args.data()), 0);
    }
    {
        EXPECT_CALL(mock, Call(testing::Truly(matches("sub1", "2"))))
            .Times(1)
            .WillOnce(testing::Return(0))
            .RetiresOnSaturation();

        auto args = std::array{"Program", "sub1"};
        ASSERT_EQ(run(root, args.size(), args.data()), 0);
    }
    {
        EXPECT_CALL(mock, Call(testing::Truly(matches("sub2", "3"))))
            .Times(1)
            .WillOnce(testing::Return(0))
            .RetiresOnSaturation();

        auto args = std::array{"Program", "sub1", "sub2"};
        ASSERT_EQ(run(root, args.size(), args.data()), 0);
    }
}
