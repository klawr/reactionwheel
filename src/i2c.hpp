#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string_view>
#include <initializer_list>

#include "thread_bus.h"

namespace reactionwheel
{

class i2c_device_id
{
public:
    explicit constexpr i2c_device_id(int value)
        : mValue(value)
    {
    }

    constexpr int value() const
    {
        return mValue;
    }

private:
    const int mValue;
};

class i2c_register
{
public:
    constexpr explicit i2c_register(std::uint8_t value)
        : mValue(value)
    {
    }

    constexpr std::uint8_t value() const
    {
        return mValue;
    }

private:
    const std::uint8_t mValue;
};

struct i2c_reg8_override_message
    : public bus_message
{
    using data_pair = std::tuple< i2c_register, std::byte >;

    i2c_reg8_override_message(data_pair data)
        : i2c_reg8_override_message({ data })
    {
    }
    i2c_reg8_override_message(std::initializer_list<data_pair> il)
        : register_values(std::move(il))
    {
    }

    std::vector< data_pair > register_values;
};

class i2c_device
{
public:
    using block_t = std::vector<std::byte>;

    explicit i2c_device(std::string_view device, i2c_device_id slave_id);
    ~i2c_device();

    void smbus_write_quick(std::byte value);
    std::byte smbus_read_byte();
    void smbus_write_byte(std::byte value);

    std::byte smbus_read_byte(i2c_register reg);
    void smbus_write_byte(i2c_register reg, std::byte value);

    std::uint16_t smbus_read_word(i2c_register reg);
    void smbus_write_word(i2c_register reg, std::uint16_t value);

    std::uint16_t smbus_process_call(i2c_register call_id, std::uint16_t value);

    void smbus_read_block(i2c_register reg, block_t &block);
    block_t smbus_read_block(i2c_register reg)
    {
        block_t block;
        smbus_read_block(reg, block);
        return block;
    }
    void smbus_write_block(i2c_register reg, const block_t &block);

    void smbus_read_i2c_block(i2c_register reg, block_t &block);
    block_t smbus_read_i2c_block(i2c_register reg)
    {
        block_t block;
        smbus_read_i2c_block(reg, block);
        return block;
    }
    void smbus_write_i2c_block(i2c_register reg, const block_t &block);

    void smbus_block_process_call(i2c_register call_id, const block_t &in, block_t &out);
    block_t smbus_block_process_call(i2c_register call_id, const block_t &in)
    {
        block_t out;
        smbus_block_process_call(call_id, in, out);
        return out;
    }

    void write(const i2c_reg8_override_message &msg)
    {
        for (const auto &[reg, val] : msg.register_values)
        {
            smbus_write_byte(reg, val);
        }
    }

private:
    int mFD;
};

}
