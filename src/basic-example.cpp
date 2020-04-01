#include <wayfire/plugin.hpp>

// Examples of various functionalities
#include "node-example.hpp"
#include "options-example.hpp"
#include "ipc-example.hpp"

class wayfire_ipc_debugger : public wf::plugin_interface_t
{
    std::unique_ptr<option_handler_t> option_handler;
    std::unique_ptr<ipc_example_t> ipc_example;

  public:
    void init() override
    {
        option_handler = std::make_unique<option_handler_t>();
        ipc_example = std::make_unique<ipc_example_t>();

        // TODO: output tracker mixin
        // TODO: output prehook for damage
        // TODO: node example
        // TODO: input grab example + custom rendering note
        // TODO: postprocessing
        // TODO: getting focused views, surfaces, toplevels
        // TODO: workspaces, workspace sets

        nlohmann::json data;
    }

    void fini() override
    {
        option_handler.reset();
        ipc_example.reset();
    }
};

DECLARE_WAYFIRE_PLUGIN(wayfire_ipc_debugger);
