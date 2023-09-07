//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <optional>
#include <string>
#include "rendering/render_context.h"

namespace vox {
/**
 * @brief An interface class, declaring the behaviour of a Window
 */
class Window {
public:
    struct Extent {
        uint32_t width;
        uint32_t height;
    };

    struct OptionalExtent {
        std::optional<uint32_t> width;
        std::optional<uint32_t> height;
    };

    enum class Mode {
        Headless,
        Fullscreen,
        FullscreenBorderless,
        FullscreenStretch,
        Default
    };

    enum class Vsync {
        OFF,
        ON,
        Default
    };

    struct OptionalProperties {
        std::optional<std::string> title;
        std::optional<Mode> mode;
        std::optional<bool> resizable;
        std::optional<Vsync> vsync;
        OptionalExtent extent;
    };

    struct Properties {
        std::string title;
        Mode mode = Mode::Default;
        bool resizable = true;
        Vsync vsync = Vsync::Default;
        Extent extent = {1280, 720};
    };

    /**
	 * @brief Constructs a Window
	 * @param properties The preferred configuration of the window
	 */
    explicit Window(Properties properties);

    virtual ~Window() = default;

    /**
     * @brief Gets a handle from the engine's Metal layer
     * @param device Device handle, for use by the application
     */
    virtual std::unique_ptr<RenderContext> createRenderContext(MTL::Device &device) = 0;

    /**
	 * @brief Checks if the window should be closed
	 */
    virtual bool should_close() = 0;

    /**
	 * @brief Handles the processing of all underlying window events
	 */
    virtual void process_events();

    /**
	 * @brief Requests to close the window
	 */
    virtual void close() = 0;

    /**
	 * @return The dot-per-inch scale factor
	 */
    [[nodiscard]] virtual float get_dpi_factor() const = 0;

    /**
	 * @return The scale factor for systems with heterogeneous window and pixel coordinates
	 */
    [[nodiscard]] virtual float get_content_scale_factor() const;

    /**
	 * @brief Attempt to resize the window - not guaranteed to change
	 *
	 * @param extent The preferred window extent
	 * @return Extent The new window extent
	 */
    Extent resize(const Extent &extent);

    [[nodiscard]] const Extent &get_extent() const;

    [[nodiscard]] Mode get_window_mode() const;

    [[nodiscard]] inline const Properties &get_properties() const {
        return properties;
    }

protected:
    Properties properties;
};
}// namespace vox
