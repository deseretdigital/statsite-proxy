#include <check.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "config.h"

START_TEST(test_config_get_default)
{
    statsite_proxy_config config;
    int res = config_from_filename(NULL, &config);
    fail_unless(res == 0);
    fail_unless(config.tcp_port == 8150);
    fail_unless(config.udp_port == 8150);
    fail_unless(strcmp(config.servers, "servers.conf") == 0);
    fail_unless(strcmp(config.log_level, "DEBUG") == 0);
    fail_unless(config.syslog_log_level == LOG_DEBUG);
    fail_unless(config.daemonize == false);
    fail_unless(strcmp(config.pid_file, "/var/run/statsite_proxy.pid") == 0);
}
END_TEST

START_TEST(test_config_bad_file)
{
    statsite_proxy_config config;
    int res = config_from_filename("/tmp/does_not_exist", &config);
    fail_unless(res == -ENOENT);

    // Should get the defaults...
    fail_unless(config.tcp_port == 8150);
	fail_unless(config.udp_port == 8150);
	fail_unless(strcmp(config.servers, "servers.conf") == 0);
	fail_unless(strcmp(config.log_level, "DEBUG") == 0);
	fail_unless(config.syslog_log_level == LOG_DEBUG);
	fail_unless(config.daemonize == false);
	fail_unless(strcmp(config.pid_file, "/var/run/statsite_proxy.pid") == 0);
}
END_TEST

START_TEST(test_config_empty_file)
{
    int fh = open("/tmp/zero_file", O_CREAT|O_RDWR, 0777);
    fchmod(fh, 777);
    close(fh);

    statsite_proxy_config config;
    int res = config_from_filename("/tmp/zero_file", &config);
    fail_unless(res == 0);

    // Should get the defaults...
    fail_unless(config.tcp_port == 8150);
	fail_unless(config.udp_port == 8150);
	fail_unless(strcmp(config.servers, "servers.conf") == 0);
	fail_unless(strcmp(config.log_level, "DEBUG") == 0);
	fail_unless(config.syslog_log_level == LOG_DEBUG);
	fail_unless(config.daemonize == false);
	fail_unless(strcmp(config.pid_file, "/var/run/statsite_proxy.pid") == 0);

    unlink("/tmp/zero_file");
}
END_TEST

START_TEST(test_config_basic_config)
{
    int fh = open("/tmp/basic_config", O_CREAT|O_RDWR|O_TRUNC, 0777);
    char *buf = "[statsite-proxy]\n\
port = 10000\n\
udp_port = 10001\n\
servers = myservers.conf\n\
log_level = INFO\n\
daemonize = true\n\
pid_file = /tmp/statsite.pid\n";
    write(fh, buf, strlen(buf));
    fchmod(fh, 777);
    close(fh);

    statsite_proxy_config config;
    int res = config_from_filename("/tmp/basic_config", &config);
    fail_unless(res == 0);

    // Should get the config
    fail_unless(config.tcp_port == 10000);
    fail_unless(config.udp_port == 10001);
    fail_unless(strcmp(config.servers, "myservers.conf") == 0,
    		"found %s should be myservers.conf", config.servers);
    fail_unless(strcmp(config.log_level, "INFO") == 0);
    fail_unless(config.daemonize == true);
    fail_unless(strcmp(config.pid_file, "/tmp/statsite.pid") == 0);

    //unlink("/tmp/basic_config");
}
END_TEST

START_TEST(test_validate_default_config)
{
    statsite_proxy_config config;
    int res = config_from_filename(NULL, &config);
    fail_unless(res == 0);

    res = validate_config(&config);
    fail_unless(res == 0);
}
END_TEST

START_TEST(test_validate_bad_config)
{
    statsite_proxy_config config;
    int res = config_from_filename(NULL, &config);
    fail_unless(res == 0);

    // Set invalid log level, should fail
    config.log_level = "INVALID_LOG_LEVEL";

    res = validate_config(&config);
    fail_unless(res == 1);
}
END_TEST

START_TEST(test_join_path_no_slash)
{
    char *s1 = "/tmp/path";
    char *s2 = "file";
    char *s3 = join_path(s1, s2);
    fail_unless(strcmp(s3, "/tmp/path/file") == 0);
}
END_TEST

START_TEST(test_join_path_with_slash)
{
    char *s1 = "/tmp/path/";
    char *s2 = "file";
    char *s3 = join_path(s1, s2);
    fail_unless(strcmp(s3, "/tmp/path/file") == 0);
}
END_TEST

START_TEST(test_sane_log_level)
{
    int log_lvl;
    fail_unless(sane_log_level("DEBUG", &log_lvl) == 0);
    fail_unless(sane_log_level("debug", &log_lvl) == 0);
    fail_unless(sane_log_level("INFO", &log_lvl) == 0);
    fail_unless(sane_log_level("info", &log_lvl) == 0);
    fail_unless(sane_log_level("WARN", &log_lvl) == 0);
    fail_unless(sane_log_level("warn", &log_lvl) == 0);
    fail_unless(sane_log_level("ERROR", &log_lvl) == 0);
    fail_unless(sane_log_level("error", &log_lvl) == 0);
    fail_unless(sane_log_level("CRITICAL", &log_lvl) == 0);
    fail_unless(sane_log_level("critical", &log_lvl) == 0);
    fail_unless(sane_log_level("foo", &log_lvl) == 1);
    fail_unless(sane_log_level("BAR", &log_lvl) == 1);
}
END_TEST
