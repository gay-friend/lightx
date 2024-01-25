#pragma once
#include "node/node.h"

#include <string>

// 获取节点类型
typedef const char *(*func_get_node_type)();
// 获取节点名
typedef const char *(*func_get_node_name)();
// 创建节点
typedef Node *(*func_create_node)(QWidget *);

static const char *NAME_OF_CREATE_NODE = "create_node";
static const char *NAME_OF_GET_NODE_NAME = "get_node_name";
static const char *NAME_OF_GET_NODE_TYPE = "get_node_type";