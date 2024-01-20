#include "utils/utils.h"
namespace fs = std::filesystem;

LibManager::LibManager(const std::string &dir) : m_dir(dir)
{
    load();
}
LibManager::~LibManager()
{
}
void LibManager::load()
{
    fs::path path(m_dir);
    if (!fs::exists(path))
    {
        return;
    }
    fs::directory_entry entry(path);
    if (entry.status().type() != fs::file_type::directory)
    {
        return;
    }
    for (auto it : fs::directory_iterator(m_dir))
    {
        auto file = it.path().string();
        std::cout << "Loading " << file << std::endl;
        QLibrary lib(QString::fromStdString(file));
        lib.load();
        if (!lib.isLoaded())
        {
            qDebug() << lib.errorString();
            continue;
        }
        auto node_name = ((func_get_node_name)lib.resolve(NAME_OF_GET_NODE_NAME))();
        auto node_type = ((func_get_node_type)lib.resolve(NAME_OF_GET_NODE_TYPE))();
        auto func_create_node_p = (func_create_node)lib.resolve(NAME_OF_CREATE_NODE);
        std::cout << "Add node " << node_type << ": " << node_name << std::endl;

        func_map[node_type][node_name] = func_create_node_p;
    }
}
void LibManager::unload()
{
    func_map.clear();
}
Node *LibManager::create_node(const std::string &node_type, const std::string &node_name)
{
    if (func_map.count(node_type) == 0 || func_map[node_type].count(node_name) == 0)
    {
        return nullptr;
    }
    return func_map[node_type][node_name](nullptr);
}
