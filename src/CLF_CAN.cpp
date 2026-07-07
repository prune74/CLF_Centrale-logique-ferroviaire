#include "CLF_CAN.h"

// Envoi brut
void CLF_CAN::sendMsg(const CanMsg &msg)
{
    CanBus::bus(0).send(msg);
}

// Construction ID 29 bits
static CanMsg makeUniversalMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID)
{
    uint32_t id29 = CanID::make29(prio, cmde, resp, trainID);
    return CanMsg(id29, {});
}

// 0 octet
void CLF_CAN::sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID)
{
    CanMsg msg = makeUniversalMsg(prio, cmde, resp, trainID);
    CanBus::bus(0).send(msg);
}

// 1 octet
void CLF_CAN::sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID,
                      uint8_t d0)
{
    CanMsg msg(CanID::make29(prio, cmde, resp, trainID), {d0});
    CanBus::bus(0).send(msg);
}

// 2 octets
void CLF_CAN::sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID,
                      uint8_t d0, uint8_t d1)
{
    CanMsg msg(CanID::make29(prio, cmde, resp, trainID), {d0, d1});
    CanBus::bus(0).send(msg);
}

// 3 octets
void CLF_CAN::sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID,
                      uint8_t d0, uint8_t d1, uint8_t d2)
{
    CanMsg msg(CanID::make29(prio, cmde, resp, trainID), {d0, d1, d2});
    CanBus::bus(0).send(msg);
}

// 4 octets
void CLF_CAN::sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID,
                      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
    CanMsg msg(CanID::make29(prio, cmde, resp, trainID), {d0, d1, d2, d3});
    CanBus::bus(0).send(msg);
}

// 5 octets
void CLF_CAN::sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID,
                      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4)
{
    CanMsg msg(CanID::make29(prio, cmde, resp, trainID), {d0, d1, d2, d3, d4});
    CanBus::bus(0).send(msg);
}

// 6 octets
void CLF_CAN::sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID,
                      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                      uint8_t d4, uint8_t d5)
{
    CanMsg msg(CanID::make29(prio, cmde, resp, trainID), {d0, d1, d2, d3, d4, d5});
    CanBus::bus(0).send(msg);
}

// 7 octets
void CLF_CAN::sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID,
                      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                      uint8_t d4, uint8_t d5, uint8_t d6)
{
    CanMsg msg(CanID::make29(prio, cmde, resp, trainID), {d0, d1, d2, d3, d4, d5, d6});
    CanBus::bus(0).send(msg);
}

// 8 octets
void CLF_CAN::sendMsg(uint8_t prio, uint8_t cmde, uint8_t resp, uint16_t trainID,
                      uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                      uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
    CanMsg msg(CanID::make29(prio, cmde, resp, trainID),
               {d0, d1, d2, d3, d4, d5, d6, d7});
    CanBus::bus(0).send(msg);
}

void CLF_CAN::sendWriteCV_Main(uint16_t locoID, uint16_t cv, uint8_t value)
{
    CanMsg msg;
    msg.id  = 0x08;   // Marklin / Locoduino
    msg.dlc = 7;

    msg.data[0] = 0x00;
    msg.data[1] = 0x00;
    msg.data[2] = (locoID >> 8) & 0xFF;
    msg.data[3] = locoID & 0xFF;
    msg.data[4] = (cv >> 8) & 0xFF;
    msg.data[5] = cv & 0xFF;
    msg.data[6] = value;

    CanBus::bus(0).send(msg);
}
