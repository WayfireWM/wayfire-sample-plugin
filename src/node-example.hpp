#pragma once

#include <wayfire/scene.hpp>
#include <wayfire/scene-input.hpp>
#include <wayfire/output.hpp>
#include <linux/input-event-codes.h>

/**
 * Wayfire's input and rendering infrastructure is organized in the scenegraph.
 * It consists of nodes, each of which may render on the screen, receive input and have its own children.
 *
 * In this example, we implement a very simple node with no children, which draws a solid colored rectangle.
 * It also registers itself to receive pointer and keyboard events.
 */
class example_simple_node_t : public wf::scene::node_t,
    public wf::keyboard_interaction_t,
    public wf::pointer_interaction_t
{
    /**
     * When a node wants to have visual output, it needs to provide render instances.
     * Each render instance represents an occurence of the node somewhere: for example, a node being dragged
     * between two outputs will have a render instance on each of them.
     *
     * Render instances can be used to achieve many different effects and are especially powerful when a node
     * wants to manipulate the appearance of its children, but such things are out of scope of this basic
     * example.
     */
    class my_render_instance_t : public wf::scene::simple_render_instance_t<example_simple_node_t>
    {
      public:
        using simple_render_instance_t::simple_render_instance_t;

        void render(const wf::render_target_t& target, const wf::region_t& region)
        {
            // Render the node on the given target and with the given damaged region.
            // The given render target has been adjusted by the node's parent's render instance so that it is
            // in the same coordinate system as the node itself.
            OpenGL::render_begin(target);
            auto g = self->get_bounding_box();
            for (auto box : region)
            {
                target.logic_scissor(wlr_box_from_pixman_box(box));
                OpenGL::render_rectangle(g, self->color, target.get_orthographic_projection());
            }

            OpenGL::render_end();
        }
    };

  public:
    // Node is red by default
    wf::color_t color{1, 0, 0, 1};

    example_simple_node_t() : node_t(false)
    {}

    void gen_render_instances(std::vector<wf::scene::render_instance_uptr>& instances,
        wf::scene::damage_callback push_damage, wf::output_t *output) override
    {
        // Render instances are recreated at many occasions when the scenegraph changes.
        instances.push_back(std::make_unique<my_render_instance_t>(this, push_damage, output));
    }

    wf::geometry_t get_bounding_box() override
    {
        // A node is free to chose its own size and position. Note however that these are set in a coordinate
        // system determined by its parent: a node's parent may apply translations, rotations, etc, so these
        // coordinates are not final on-screen coordinates.
        return {0, 0, 1280, 720};
    }

    // This example node wants to interact with the keyboard. For this reason, we need to specify where the
    // keyboard handling is implemented (here the node also implements keyboard_interaction_t)
    wf::keyboard_interaction_t& keyboard_interaction() override
    {
        return *this;
    }

    void handle_keyboard_enter(wf::seat_t*) override
    {}

    void handle_keyboard_leave(wf::seat_t*) override
    {}

    void handle_keyboard_key(wf::seat_t*, wlr_keyboard_key_event event) override
    {
        if (event.keycode == KEY_T && event.state == WL_KEYBOARD_KEY_STATE_PRESSED)
        {
            // do something about it
        }
    }

    // One last, but very important operation for keyboard input is the refocus operation.
    // It is the operation which dictates which node gets keyboard focus at any given moment.
    // Nodes can request different focus priorities.
    wf::keyboard_focus_node_t keyboard_refocus(wf::output_t *output) override
    {
        // The example plugin adds the node directly as a child of the scenegraph root.
        // This way, we know that our bounding box is defined in the global coordinate system,
        // and we can thus check whether we are visible on a particular output.
        if (!(output->get_layout_geometry() & get_bounding_box()))
        {
            // Not visible on the output
            return wf::keyboard_focus_node_t {
                .node = NULL,
                .importance = wf::focus_importance::NONE,
                .allow_focus_below = true,
            };
        }

        // We request regular focus importance. Note that among all nodes with the same importance, core will
        // select the one with the highest last_focus_timestamp.
        return wf::keyboard_focus_node_t {
            .node = this,
            .importance = wf::focus_importance::REGULAR,
            .allow_focus_below = true,
        };
    }

    // This example node also wants to interact with the pointer. For this reason, we need to specify where the
    // pointer handling is implemented (here the node also implements pointer_interaction_t)
    // Note that the scenegraph convention is that every node transforms input event for its children.
    // This means that enter/motion events contain the position in the node's own coordinate system, not in
    // global on-screen coordinates.
    wf::pointer_interaction_t& pointer_interaction() override
    {
        return *this;
    }

    // First, a node should specify where it wants to receive input.
    // The find_node_at function takes a coordinate in the node's coordinate system (i.e with the
    // transformation of the parent nodes applied, same as bounding box, etc.) and needs to return which
    // node should get a focus for that region.
    //
    // Note that if a node with higher Z-order obstructs a part of the node, that other node will get pointer
    // input first.
    std::optional<wf::scene::input_node_t> find_node_at(const wf::pointf_t& at) override
    {
        // Simply check whether the mouse is over our bounding box.
        wf::region_t our_region{get_bounding_box()};
        if (our_region.contains_pointf(at))
        {
            wf::scene::input_node_t result;
            result.node = this;
            result.local_coords = at;
            return result;
        }

        return {};
    }

    void handle_pointer_enter(wf::pointf_t position) override
    {
        (void)position;
    }

    void handle_pointer_leave() override
    {}

    void handle_pointer_button(const wlr_pointer_button_event& event) override
    {
        (void)event;
    }

    void handle_pointer_motion(wf::pointf_t pointer_position, uint32_t time_ms) override
    {
        (void)pointer_position;
        (void)time_ms;
    }

    void handle_pointer_axis(const wlr_pointer_axis_event& event) override
    {
        (void)event;
    }
};
