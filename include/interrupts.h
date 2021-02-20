/*
    interrupts.h - Header file for x86 interrupts 
*/

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "common.h"
#include "types.h"
#include "port.h"
#include "gdt.h"

// IDT macros
#define NUM_INTERRUPTS 256 // number of entries in IDT (max 256 in x86)
#define HW_INTERRUPT_OFFSET 0x20 // good practice to leave the first 32 IRQs for system interrupts
#define IDT_INTERRUPT_GATE 0xE // Descriptor type for interrupt gates
#define IDT_TRAP_GATE 0xF // Descriptor type for trap gates
#define IDT_DESC_PRESENT 0x80  // Flag to confirm IDT entry is present

// PIC macros
#define PIC_MASTER_COMMAND_PORT 0x20
#define PIC_SLAVE_COMMAND_PORT 0xA0
#define PIC_MASTER_DATA_PORT 0x21
#define PIC_SLAVE_DATA_PORT 0xA1
#define EOI 0x20 // End Of Interrupt command

class interruptsHandler;

// Non-static class that encapsulates the ISR for each IRQ
class interruptHandle {
protected:
    uint8_t IRQ;
    interruptsHandler *InterruptsHandler;
    interruptHandle(interruptsHandler *InterruptsHandler, uint8_t IRQ);
    ~interruptHandle();
 
 public:
    // Virtual function so it can be overloaded by drivers
    virtual uint32_t ISR(uint32_t esp);
};

// Static class to setup the IDT and handle IRQs
class interruptsHandler {

    friend class interruptHandle;

protected:
    static interruptsHandler* ActiveInterruptsHandler; // Not great, but required to get out of static space
    interruptHandle* interruptHandles[NUM_INTERRUPTS]; // Pointers to interruptHandle objects

    // IDT gate (entry) definition
    struct Gate {
        uint16_t offsetLo;
        uint16_t selector;
        uint8_t zero;
        uint8_t typeAttr;
        uint16_t offsetHi;
    } __attribute__((packed));

    // IDT
    static Gate interruptDescriptorTable[NUM_INTERRUPTS];

    // IDT pointer
    struct interruptDescriptorTablePointer {
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));

    // Sets the IDT entry for the specified IRQ
    static void SetInterruptDescriptorTableEntry(
        uint8_t IRQ,
        uint16_t codeSegmentSelectorOffset, 
        void (*handler)(),
        uint8_t DescriptorPrivilegeLevel, 
        uint8_t DescriptorType);

    // Interrupt handlers
    // 0x00...0x0F are system reserved interrupts
    static void HandlerIRQ0x00();
    static void HandlerIRQ0x01();
    static void HandlerIRQ0x02();
    static void HandlerIRQ0x03();
    static void HandlerIRQ0x04();
    static void HandlerIRQ0x05();
    static void HandlerIRQ0x06();
    static void HandlerIRQ0x07();
    static void HandlerIRQ0x08();
    static void HandlerIRQ0x09();
    static void HandlerIRQ0x0A();
    static void HandlerIRQ0x0B();
    static void HandlerIRQ0x0C();
    static void HandlerIRQ0x0D();
    static void HandlerIRQ0x0E();
    static void HandlerIRQ0x0F();
    
    static void InterruptIgnore(); // Handler for non-handled IRQs

    // Exception handlers
    static void HandlerException0x00();
    static void HandlerException0x01();
    static void HandlerException0x02();
    static void HandlerException0x03();
    static void HandlerException0x04();
    static void HandlerException0x05();
    static void HandlerException0x06();
    static void HandlerException0x07();
    static void HandlerException0x08();
    static void HandlerException0x09();
    static void HandlerException0x0A();
    static void HandlerException0x0B();
    static void HandlerException0x0C();
    static void HandlerException0x0D();
    static void HandlerException0x0E();
    static void HandlerException0x0F();
    static void HandlerException0x10();
    static void HandlerException0x11();
    static void HandlerException0x12();
    static void HandlerException0x13();

    // Top level IRQ handler - called for every interrupt which then calls the below non-static method
    static uint32_t HandleInterrupt(uint8_t IRQ, uint32_t esp);
    
    // Non static version of the above method
    uint32_t DoHandleInterrupt(uint8_t IRQ, uint32_t esp);

    // PIC ports
    Port8Bit PICMasterCommandPort;
    Port8Bit PICSlaveCommandPort;
    Port8Bit PICMasterDataPort;
    Port8Bit PICSlaveDataPort;

public:
    interruptsHandler(GlobalDescriptorTable* globalDescriptorTable);
    ~interruptsHandler();
    void Activate();
    void Deactivate();
};

#endif // INTERRUPTS_H