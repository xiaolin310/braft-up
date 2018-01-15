// libraft - Quorum-based replication of states across machines.
// Copyright (c) 2018 Baidu.com, Inc. All Rights Reserved

// Author: Zhangyi Chen (chenzhangyi01@baidu.com)
// Date: 2018/01/12 23:05:44

#include <map>                  // std::map
#include <gflags/gflags.h>      // google::ParseCommandLineFlags
#include <raft/cli.h>           // raft::cli::*
#include <base/string_printf.h>

namespace raft {
namespace cli {

DEFINE_int32(timeout_ms, -1, "Timeout (in milliseconds) of the operation");
DEFINE_int32(max_retry, 3, "Max retry times of each operation");
DEFINE_string(conf, "", "Configuration of the raft group");
DEFINE_string(peer, "", "Id of the operating peer");
DEFINE_string(group, "", "Id of the raft group");

#define CHECK_FLAG(flagname)                                            \
    do {                                                                \
        if ((FLAGS_ ## flagname).empty()) {                             \
            LOG(ERROR) << __FUNCTION__ << " requires --" # flagname ;   \
            return -1;                                                  \
        }                                                               \
    } while (0);                                                        \

int add_peer() {
    CHECK_FLAG(conf);
    CHECK_FLAG(peer);
    CHECK_FLAG(group);
    Configuration conf;
    if (conf.parse_from(FLAGS_conf) != 0) {
        LOG(ERROR) << "Fail to parse --conf=`" << FLAGS_conf << '\'';
        return -1;
    }
    PeerId new_peer;
    if (new_peer.parse(FLAGS_peer) != 0) {
        LOG(ERROR) << "Fail to parse --peer=`" << FLAGS_peer<< '\'';
        return -1;
    }
    CliOptions opt;
    opt.timeout_ms = FLAGS_timeout_ms;
    opt.max_retry = FLAGS_max_retry;
    base::Status st = add_peer(FLAGS_group, conf, new_peer, opt);
    if (!st.ok()) {
        LOG(ERROR) << "Fail to add_peer : " << st;
        return -1;
    }
    return 0;
}

int remove_peer() {
    CHECK_FLAG(conf);
    CHECK_FLAG(peer);
    CHECK_FLAG(group);
    Configuration conf;
    if (conf.parse_from(FLAGS_conf) != 0) {
        LOG(ERROR) << "Fail to parse --conf=`" << FLAGS_conf << '\'';
        return -1;
    }
    PeerId removing_peer;
    if (removing_peer.parse(FLAGS_peer) != 0) {
        LOG(ERROR) << "Fail to parse --peer=`" << FLAGS_peer<< '\'';
        return -1;
    }
    CliOptions opt;
    opt.timeout_ms = FLAGS_timeout_ms;
    opt.max_retry = FLAGS_max_retry;
    base::Status st = remove_peer(FLAGS_group, conf, removing_peer, opt);
    if (!st.ok()) {
        LOG(ERROR) << "Fail to remove_peer : " << st;
        return -1;
    }
    return 0;
}

int set_peer() {
    CHECK_FLAG(conf);
    CHECK_FLAG(peer);
    CHECK_FLAG(group);
    Configuration conf;
    if (conf.parse_from(FLAGS_conf) != 0) {
        LOG(ERROR) << "Fail to parse --conf=`" << FLAGS_conf << '\'';
        return -1;
    }
    PeerId target_peer;
    if (target_peer.parse(FLAGS_peer) != 0) {
        LOG(ERROR) << "Fail to parse --peer=`" << FLAGS_peer<< '\'';
        return -1;
    }
    CliOptions opt;
    opt.timeout_ms = FLAGS_timeout_ms;
    opt.max_retry = FLAGS_max_retry;
    base::Status st = set_peer(FLAGS_group, target_peer, conf, opt);
    if (!st.ok()) {
        LOG(ERROR) << "Fail to set_peer : " << st;
        return -1;
    }
    return 0;
}

int snapshot() {
    CHECK_FLAG(peer);
    CHECK_FLAG(group);
    PeerId target_peer;
    if (target_peer.parse(FLAGS_peer) != 0) {
        LOG(ERROR) << "Fail to parse --peer=`" << FLAGS_peer<< '\'';
        return -1;
    }
    CliOptions opt;
    opt.timeout_ms = FLAGS_timeout_ms;
    opt.max_retry = FLAGS_max_retry;
    base::Status st = snapshot(FLAGS_group, target_peer, opt);
    if (!st.ok()) {
        LOG(ERROR) << "Fail to make snapshot : " << st;
        return -1;
    }
    return 0;
}

int run_command(const std::string& cmd) {
    if (cmd == "add_peer") {
        return add_peer();
    }
    if (cmd == "remove_peer") {
        return remove_peer();
    }
    if (cmd == "set_peer") {
        return set_peer();
    }
    if (cmd == "snapshot") {
        return snapshot();
    }
    LOG(ERROR) << "Unknown command `" << cmd << '\'';
    return -1;
}

}  // namespace cli
}  // namespace raft

int main(int argc , char* argv[]) {
    const char* proc_name = strrchr(argv[0], '/');
    if (proc_name == NULL) {
        proc_name = argv[0];
    } else {
        ++proc_name;
    }
    std::string help_str;
    base::string_printf(&help_str,
                        "Usage: %s [Command] [OPTIONS...]\n"
                        "Command:\n"
                        "  add_peer --group=$group_id "
                                    "--peer=$adding_peer --conf=$current_conf\n"
                        "  remove_peer --group=$group_id "
                                      "--peer=$removing_peer --conf=$current_conf\n"
                        "  set_peer --group=$group_id "
                                   "--peer==$target_peer --conf=$target_conf\n"
                        "  snapshot --group=$group_id --peer=$target_peer\n",
                        proc_name);
    google::SetUsageMessage(help_str);
    google::ParseCommandLineFlags(&argc, &argv, true);
    if (argc != 2) {
        std::cerr << help_str;
        return -1;
    }
    return raft::cli::run_command(argv[1]);
}
