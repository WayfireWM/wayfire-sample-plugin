#pragma once

#include <wayfire/plugins/common/shared-core-data.hpp>
#include <wayfire/plugins/ipc/ipc-method-repository.hpp>
#include <wayfire/plugins/ipc/ipc-activator.hpp>
#include <wayfire/plugins/ipc/ipc-helpers.hpp>
#include <wayfire/plugins/ipc/ipc.hpp>
#include <wayfire/core.hpp>
#include <wayfire/seat.hpp>
#include <algorithm>

/**
 * An example of how to use the IPC functionality provided by Wayfire.
 * As in some of the other examples, the code could easily be moved to the plugin main class.
 */
class ipc_example_t
{
    // The method repository is a global object (which can be accessed by the ref_ptr_t helper) which contains
    // all currently available IPC methods.
    wf::shared_data::ref_ptr_t<wf::ipc::method_repository_t> repo;

    // We also need a reference to the IPC socket server itself (for this example).
    wf::shared_data::ref_ptr_t<wf::ipc::server_t> ipc_server;

  public:
    ipc_example_t()
    {
        // The IPC socket itself is provided by the IPC plugin, if enabled.
        // Plugins can register their own methods, which other plugins or other clients can call via the
        // socket. The data follows a simple json format, but plugins don't need to handle the encoding
        // themselves.
        //
        // In this example, we provide an IPC method called client-interest.
        // IPC client can call it and either request one-shot information about the compositor state,
        // or request to be subscribed to future events. In the latter case, they are added to a list of
        // subscribed IPC clients, and are notified when something happens.
        repo->register_method("basic-example/client-interest", handle_client_interest);

        // For clients which were added to the subscriber list, we need to remove them once they disconnect.
        ipc_server->connect(&on_client_disconnect);

        // Another way to use the IPC socket is to combine an ipc method with an activator binding.
        // The binding should be available in the XML file.
        //
        // Then, the plugin can have the same handling code for the activator binding and any calls from
        // other plugins or from the IPC socket.
        ipc_activator.set_handler(activator_option_handler);

        // And of course, we can call other plugins' IPC methods
        // Here for example we can toggle scale.
        if (false)
        {
            nlohmann::json data;
            data["output_id"] = wf::get_core().seat->get_active_output()->get_id();
            repo->call_method("scale/toggle", data);
        }
    }

    ~ipc_example_t()
    {
        repo->unregister_method("basic-example/client-interest");
    }

    std::vector<wf::ipc::client_t*> subscribers_list;

    // A handler for an IPC method call. `data` is the data which comes from the client, the return value
    // is sent to it as a response.
    wf::ipc::method_callback handle_client_interest = [=] (const nlohmann::json& data)
    {
        // A helper macro to verify that the client has sent the necessary data.
        // It also verifies the data type. See json::is_{string,number,...}()
        WFJSON_EXPECT_FIELD(data, "type", string);

        // Optional arguments may not be specified, but if they are specified, ensure they have the said type.
        WFJSON_OPTIONAL_FIELD(data, "optional", number_integer);

        if (data["type"] == "oneshot")
        {
            // The client does not want to 'subscribe' to events, just return some info it needs
            // Of course, we can return any information here or execute an action.
            // Most IPC commands work in this mode only.
            nlohmann::json response = wf::ipc::json_ok();
            response["nr-subscribers"] = subscribers_list.size();
            return response;
        } else if (data["type"] == "subscribe")
        {
            // Client wants to be added in a list of subscribers.
            if (ipc_server->get_current_request_client())
            {
                // We can query the current client from the ipc server, so that we can add it to the list.
                // Note that the current client might be NULL if another plugin has sent us the request.
                // In this case, there is nothing to subscribe to.
                subscribers_list.push_back(ipc_server->get_current_request_client());
            }

            auto response = notify_subscribers();
            return response;
        } else
        {
            return wf::ipc::json_error("Invalid interest type!");
        }
    };

    nlohmann::json notify_subscribers()
    {
        nlohmann::json response = wf::ipc::json_ok();
        response["event"] = "nr-subscribers-changed";
        response["nr-subscribers"] = subscribers_list.size();

        // Notify all previous subscribers that the number of subscribers has changed.
        for (auto sub : subscribers_list)
        {
            sub->send_json(response);
        }

        return response;
    }

    // A handler for the case when an ipc client is disconnected.
    // If it had a subscription, we need to remove it.
    wf::signal::connection_t<wf::ipc::client_disconnected_signal> on_client_disconnect =
        [=] (wf::ipc::client_disconnected_signal *ev)
    {
        auto it = std::remove(subscribers_list.begin(), subscribers_list.end(), ev->client);
        subscribers_list.erase(it, subscribers_list.end());
    };

    wf::ipc_activator_t ipc_activator{"basic-example/activator_option"};
    wf::ipc_activator_t::handler_t activator_option_handler = [=] (wf::output_t *output, wayfire_view view)
    {
        // The IPC activator provides limited options passed to the activator handler
        // The output is the currently focused Wayfire output, but if the handler is invoked from the IPC
        // socket, the caller can override the given output.
        //
        // For activations from a button binding, the view will be the view under the cursor.
        // For other activator binding types, the view will be the currently focused view.
        // For activations from IPC, the view can be set by the caller itself.

        (void)output;
        (void)view;

        // The return value is used for activator binding activations only: true if the triggering input event
        // was consumed by the handler, false otherwise.
        return true;
    };
};
