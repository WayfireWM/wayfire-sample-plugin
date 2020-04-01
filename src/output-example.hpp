#pragma once

#include <wayfire/core.hpp>
#include <wayfire/output-layout.hpp>
#include <wayfire/per-output-plugin.hpp>
#include <wayfire/workarea.hpp>

/**
 * This class contains an example of some aspects of output handling in Wayfire.
 *
 * Note: we use the base class per_output_tracker_mixin_t.
 * This class can be used to more easily keep track of outputs, as can be seen in the code below.
 */
class output_example_t : public wf::per_output_tracker_mixin_t<>
{
  public:
    output_example_t()
    {
        // We can use output-layout's methods to manipulate outputs in Wayfire.
        auto output_config = wf::get_core().output_layout->get_current_configuration();
        for (wf::output_t *wo : wf::get_core().output_layout->get_outputs())
        {
            // Here, we print a list of all outputs.
            // The mode of an output is the final resolution of the screen.
            // But the logical geometry is usually used for most operations concerning window size,
            // output placement etc.
            //
            // For example, a rotated 4k monitor (with scale 2, 90 degrees rotation) would have a mode
            // 3840x2160, but the logical size would be 1080x1920.
            LOGI("We have output ", wo->to_string(), " with mode ",
                output_config[wo->handle].mode.width, "x", output_config[wo->handle].mode.height,
                " and with logical geometry ", wo->get_layout_geometry(),
                // The workarea is the (logical) area of the output not covered by panels and bars.
                " available workarea is ", wo->workarea->get_workarea());
        }

    }

    ~output_example_t()
    {
    }

    // A method overridden from the output tracker mixin. Called once for every existing output
    // and for every subsequently plugged in output.
    void handle_new_output(wf::output_t *output) override
    {
    }

    // A method overridden from the output tracker mixin. Called once for every output which was unplugged.
    void handle_output_removed(wf::output_t *output) override
    {
    }
};
