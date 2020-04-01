#pragma once

#include <wayfire/bindings.hpp>
#include <wayfire/config/option-wrapper.hpp>
#include <wayfire/config/types.hpp>
#include <wayfire/option-wrapper.hpp>
#include <wayfire/config/compound-option.hpp>
#include <wayfire/util/log.hpp>
#include <wayfire/core.hpp>
#include <wayfire/bindings-repository.hpp>

/**
 * This class shows how to use the options defined in the plugin XML file.
 * Note that the code can easily be placed inside the plugin's main class, but we keep it separately here to
 * avoid cluttering the main class too much.
 */
class option_handler_t
{
    // The options will be loaded as soon as an instance of this class is created.
    wf::option_wrapper_t<int> int_opt{"basic-example/int_option"};
    wf::option_wrapper_t<double> double_opt{"basic-example/double_option"};
    wf::option_wrapper_t<bool> bool_opt{"basic-example/bool_option"};
    wf::option_wrapper_t<std::string> string_opt{"basic-example/string_option"};
    wf::option_wrapper_t<wf::activatorbinding_t> activator_opt{"basic-example/activator_option"};

    wf::option_wrapper_t<wf::config::compound_list_t<wf::activatorbinding_t, std::string, std::string>>
        list_opt{"basic-example/list_opt"};

  public:
    option_handler_t()
    {
        // We can now use the options however we want ...
        if (int_opt == 5) // If the type is unclear, you can also explicitly cast the option: (int)int_opt
        {
            LOGI("basic-exmaple/int_option is 5!");
        } else
        {
            LOGI("basic-exmaple/int_option is not 5!");
        }

        // List options are stored as tuples with their name in the config file
        for (auto& [name, binding, command, type] : list_opt.value())
        {
            LOGI("List entry name=", name, " binding=", binding, " command=", command, " type=", type);
        }

        // For activators, we usually want to register the bindings in core. We pass the option to core, so
        // that it can always fetch the newest value from the config file for the binding.
        //
        // Another way to do this is via an ipc handler (see the ipc example for a detailed explanation).
        wf::get_core().bindings->add_activator(activator_opt, &on_activator_triggered);

        // Advanced: We can also override options and 'lock' them so that the value set by the plugin is not
        // overridden by config reloading. Most plugins don't need this, but it can be useful if a plugin
        // wants to influence another plugin.
        //
        // Note that if a plugin wants to replace the whole config file, usually you want a config backend
        // plugin instead of a normal plugin.
        auto as_raw_opt = (wf::option_sptr_t<bool>)bool_opt;
        as_raw_opt->set_locked();
        as_raw_opt->set_value(true);
    }

    ~option_handler_t()
    {
        // Don't forget to clean up bindings
        wf::get_core().bindings->rem_binding(&on_activator_triggered);
    }

    wf::activator_callback on_activator_triggered = [=] (const wf::activator_data_t& data)
    {
        // Handle the activator
        switch (data.source)
        {
          case wf::activator_source_t::KEYBINDING:
            LOGI("Plugin activated with key ", data.activation_data);
            break;
          case wf::activator_source_t::MODIFIERBINDING:
            LOGI("Plugin activated with modifier ", data.activation_data);
            break;
          case wf::activator_source_t::BUTTONBINDING:
            LOGI("Plugin activated with button ", data.activation_data);
            break;
          default:
            break;
        };

        // If we return true, the key/button/whatever will not be sent to clients.
        return true;
    };
};
