#include <nil/clix.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace
{
    struct MockCall
    {
        MockCall()
        {
            global = &mock;
        }

        ~MockCall()
        {
            global = nullptr;
        }

        MockCall(MockCall&&) = delete;
        MockCall(const MockCall&) = delete;
        MockCall& operator=(MockCall&&) = delete;
        MockCall& operator=(const MockCall&) = delete;

        using type = testing::StrictMock<testing::MockFunction<int(const nil::clix::Options&)>>;
        type mock;
        static type* global;
    };

    MockCall::type* MockCall::global = nullptr;

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

    void apply(nil::clix::Node& node)
    {
        node.flag("flag", {.skey = 'f', .msg = "flag msg"});
        node.number("number", {.skey = 'n', .msg = "number msg"});
        node.param("param", {.skey = 'p', .msg = "param msg", .fallback = "default value"});
        node.params("mparam", {.skey = 'm', .msg = "mparam msg"});
        node.runner( //
            [](const nil::clix::Options& options) { return MockCall::global->Call(options); }
        );
    }
} // namespace

TEST(cli, base)
{
    auto args = std::array{"Program"};
    ASSERT_EQ(nil::clix::Node().run(args.size(), args.data()), 0);
}

TEST(cli, depth_one)
{
    MockCall called;

    const auto matches = [](const nil::clix::Options& output)
    {
        return !output.flag("flag")                     //
            && output.number("number") == 0             //
            && output.param("param") == "default value" //
            && output.params("mparam").empty();
    };
    EXPECT_CALL(called.mock, Call(testing::Truly(matches)))
        .Times(1)
        .WillOnce(testing::Return(0))
        .RetiresOnSaturation();

    auto args = std::array{"Program"};

    nil::clix::Node node;
    apply(node);
    apply(node);
    ASSERT_EQ(node.run(args.size(), args.data()), 0);
}

TEST(cli, depth_deep)
{
    MockCall called;

    nil::clix::Node root;

    apply(root);

    root.add(
        "sub1",
        "description of sub1",
        [](nil::clix::Node& sub1)
        {
            apply(sub1);
            sub1.add(
                "sub2",
                "description of sub2",
                [](nil::clix::Node& sub2)
                {
                    apply(sub2);
                    sub2.add("sub3", "description of sub3", apply);
                }
            );
        }
    );

    const auto matches = [](std::string h)
    {
        return [h = std::move(h)](const nil::clix::Options& output)
        {
            std::ostringstream oss;
            output.help(oss);
            return oss.str() == help(h)                     //
                && !output.flag("flag")                     //
                && output.number("number") == 0             //
                && output.param("param") == "default value" //
                && output.params("mparam").empty();
        };
    };
    {
        EXPECT_CALL(called.mock, Call(testing::Truly(matches("1"))))
            .Times(1)
            .WillOnce(testing::Return(0))
            .RetiresOnSaturation();

        auto args = std::array{"Program"};
        ASSERT_EQ(root.run(args.size(), args.data()), 0);
    }
    {
        EXPECT_CALL(called.mock, Call(testing::Truly(matches("2"))))
            .Times(1)
            .WillOnce(testing::Return(0))
            .RetiresOnSaturation();

        auto args = std::array{"Program", "sub1"};
        ASSERT_EQ(root.run(args.size(), args.data()), 0);
    }
    {
        EXPECT_CALL(called.mock, Call(testing::Truly(matches("3"))))
            .Times(1)
            .WillOnce(testing::Return(0))
            .RetiresOnSaturation();

        auto args = std::array{"Program", "sub1", "sub2"};
        ASSERT_EQ(root.run(args.size(), args.data()), 0);
    }
}
