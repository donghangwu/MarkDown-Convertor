#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig)
{
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("config_tests/example_config", &out_config);

  EXPECT_TRUE(success);
}

class NginxConfigParserTestFixture : public ::testing::Test
{
protected:
  NginxConfigParser parser;
  NginxConfig out_config;
};

TEST_F(NginxConfigParserTestFixture, config_location1)
{
  bool success = parser.Parse("config_tests/config_location1", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, config_location2)
{
  bool success = parser.Parse("config_tests/config_location2", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, config_location3)
{
  bool success = parser.Parse("config_tests/config_location3", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, config_location4)
{
  bool success = parser.Parse("config_tests/config_location4", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, config_listen1)
{
  bool success = parser.Parse("config_tests/config_listen1", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, config_listen2)
{
  bool success = parser.Parse("config_tests/config_listen2", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, config_proxy)
{
  bool success = parser.Parse("config_tests/config_proxy", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, empty_config)
{
  bool success = parser.Parse("config_tests/empty_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, config_server1)
{
  bool success = parser.Parse("config_tests/config_server1", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTestFixture, incorrent_config1)
{
  bool success = parser.Parse("config_tests/incorrect_config1", &out_config);
  EXPECT_FALSE(success);
}
TEST_F(NginxConfigParserTestFixture, incorrent_config2)
{
  bool success = parser.Parse("config_tests/incorrect_config2", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestFixture, incorrent_config3)
{
  bool success = parser.Parse("config_tests/incorrect_config3", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestFixture, incorrent_config4)
{
  bool success = parser.Parse("config_tests/incorrect_config4", &out_config);

  EXPECT_FALSE(success);
}
TEST_F(NginxConfigParserTestFixture, config_comment)
{
  bool success = parser.Parse("config_tests/config_comment", &out_config);

  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestFixture, config_get_port_listen)
{
  // bool success = parser.Parse("config_tests/config_get_port1", &out_config);

  parser.Parse("config_tests/config_get_port_listen", &out_config);

  int port_num = out_config.GetPortNum();

  EXPECT_TRUE(port_num == 8080);
}

TEST_F(NginxConfigParserTestFixture, config_get_port_port)
{
  // bool success = parser.Parse("config_tests/config_get_port1", &out_config);

  parser.Parse("config_tests/config_get_port_port", &out_config);

  int port_num = out_config.GetPortNum();

  EXPECT_TRUE(port_num == 80);
}

TEST_F(NginxConfigParserTestFixture, config_no_port)
{
  // bool success = parser.Parse("config_tests/config_get_port1", &out_config);

  parser.Parse("config_tests/config_no_port", &out_config);

  int port_num = out_config.GetPortNum();

  EXPECT_TRUE(port_num == -1);
}

TEST_F(NginxConfigParserTestFixture, config_single_quote_escape)
{
  // bool success = parser.Parse("config_tests/config_get_port1", &out_config);

  bool success = parser.Parse("config_tests/config_single_quote_escape", &out_config);

  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTestFixture, config_fail_to_open_file)
{
  // bool success = parser.Parse("config_tests/config_get_port1", &out_config);

  bool success = parser.Parse("config_tests/non_existing_file", &out_config);

  EXPECT_FALSE(success);
}
