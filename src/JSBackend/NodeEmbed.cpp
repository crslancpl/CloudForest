#include "NodeEmbed.h"

#include "node/node.h"
#include <nodejs/src/node.h>
#include <uv.h>

#include <memory>
#include <assert.h>
#include <algorithm>

using namespace std;


void SetupNodeJS(){
    //uv_setup_args(int argc, char **argv)
    node::InitializeNodeWithArgs(nullptr, nullptr, nullptr);
    //node::Environment *env = node::CreateEnvironment();
}
