/*
    mouse.h - mouse driver for x86 OS kernel
*/

#ifndef MOUSE_H
#define MOUSE_H

#include "common/types.h"
#include "drivers/driver.h"
#include "kernel/port.h"
#include "kernel/interrupts.h"
#include "common/common.h"
#include "drivers/vga.h"

namespace morphios {
namespace drivers {

// Mouse event handler class
class MouseEventHandler {
protected:
    morphios::common::int8_t x, y;
    morphios::common::uint8_t buttonsStatus;
    // Typecast to signed int8
    static const morphios::common::int8_t SCREEN_W = (morphios::common::int8_t)VGA::VGA_TEXT_MODE_WIDTH;
    static const morphios::common::int8_t SCREEN_H = (morphios::common::int8_t)VGA::VGA_TEXT_MODE_HEIGHT;
    void switchFGandBGcolours();

public:
    MouseEventHandler();
    ~MouseEventHandler();
    virtual void onMouseActivate();
    virtual void onMouseDown(common::uint8_t button);
    virtual void onMouseUp(common::uint8_t button);
    virtual void onMouseMove(morphios::common::int32_t xOffset, morphios::common::int32_t yOffset);
};

class MouseDriver : public morphios::kernel::interruptHandle, public Driver {
protected:
    morphios::kernel::Port8Bit dataPort;
    morphios::kernel::Port8Bit commandPort;
    morphios::common::uint8_t buffer[3];
    morphios::common::uint8_t offset;
    morphios::common::uint8_t buttons;
    MouseEventHandler *eventHandler;

public:
    MouseDriver(morphios::kernel::interruptsHandler *IRQhandler, MouseEventHandler *eventhandler);
    ~MouseDriver();    
    virtual morphios::common::uint32_t ISR(morphios::common::uint32_t esp);
    virtual void activate();
};

} // namespace drivers    
} // namespace morphios

#endif // MOUSE_H