#include "i2c.hpp"

#include <cerrno>
#include <cstdint>

#include <string>
#include <stdexcept>
#include <system_error>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

namespace reactionwheel
{

namespace
{

namespace i2c
{

enum class msg_flags : std::uint16_t
{
    rd = 0x01,
    nostart = 0x4000,
    rev_dir_addr = 0x2000,
    ignore_nak = 0x1000,
    no_rd_ack = 0x0800,
};

struct raw_msg
{
    std::uint16_t address;
    msg_flags flags;
    std::int16_t length;
    std::byte *buffer;
};

constexpr unsigned long open_slave = 0x0703;
constexpr unsigned long bus_cmd = 0x0720;
enum class ioctl_cmd : unsigned long
{
    // set number of retries a device should be polled
    // when not acknowledging
    retries = 0x0701,
    // set timeout in 10ms units
    timeout = 0x0702,
    // set slave address
    slave = 0x0703,
    // set slave address even if already in use
    slave_force = 0x0706,
    // combined read write transfer
    rdwr = 0x070,
    // smbus transfer
    smbus = 0x0720,
};

template< typename... Args >
inline int safe_ioctl(int fd, ioctl_cmd cmd, const std::string &err_msg, Args... args)
{
    if (int rv = ioctl(fd, static_cast<unsigned long>(cmd),
            std::forward<Args>(args)...);
        rv >= 0)
    {
        return rv;
    }
    throw std::system_error(errno, std::system_category(),
        err_msg);
}

enum class smbus_cmd : uint8_t
{
    write = 0,
    read = 1,
};

enum class smbus_tx_type :std::uint32_t
{
    quick = 0,
    byte = 1,
    byte_data = 2,
    word_data = 3,
    proc_call = 4,
    block_data = 5,
    i2c_block_broken = 6,
    block_proc_call = 7,
    i2c_block_data = 8,
};

union smbus_packet
{
    static constexpr std::size_t max_block_size = 32;

    std::byte byte;
    std::uint16_t word;
    // block[0] is used for length, one more for pec
    std::uint8_t block[max_block_size + 2];
};

struct smbus_ioctl_data
{
    smbus_cmd cmd;
    std::uint8_t reg;
    smbus_tx_type tx_type;
    smbus_packet *data;

    inline int do_call(int fd, const std::string &error_msg)
    {
        return safe_ioctl(fd, ioctl_cmd::smbus, error_msg, this);
    }
};

struct rdwr_ioctl_data
{
    static constexpr std::uint32_t max_msgs = 42;

    raw_msg *msgs;
    std::uint32_t nmsgs;
};

}

}

i2c_device::i2c_device(std::string_view device, i2c_device_id slaveId)
{
    const char *devPath;
    std::string memHolder;
    if (device.back() != '\0')
    {
        memHolder = device;
        devPath = memHolder.c_str();
    }
    else
    {
        devPath = device.data();
    }

    mFD = open(devPath, O_RDWR);
    if (mFD < 0)
    {
        throw std::system_error(errno, std::system_category(),
            "failed to open the hardware device");
    }
    try
    {
        i2c::safe_ioctl(mFD, i2c::ioctl_cmd::slave,
            "failed to select the i2c slave device",
            slaveId.value());
    }
    catch (...)
    {
        close(mFD);
        mFD = -1;
        throw;
    }
}

i2c_device::~i2c_device()
{
    if (mFD >= 0)
    {
        close(mFD);
    }
}

void i2c_device::smbus_write_quick(std::byte value)
{
    using namespace i2c;

    smbus_ioctl_data {
        static_cast<smbus_cmd>(value), // quick value passed in here
        0, // reg
        smbus_tx_type::quick,
        nullptr,
    }.do_call(mFD, "Failed to do i2c smbus quick write");
}

std::byte i2c_device::smbus_read_byte()
{
    using namespace i2c;

    smbus_packet data;
    smbus_ioctl_data {
        smbus_cmd::read,
        0,
        smbus_tx_type::byte,
        &data,
    }.do_call(mFD, "Failed to do i2c smbus byte read");
    return data.byte;
}
void i2c_device::smbus_write_byte(std::byte value)
{
    using namespace i2c;

    smbus_ioctl_data {
        smbus_cmd::write,
        std::to_integer<std::uint8_t>(value),
        smbus_tx_type::byte,
        nullptr,
    }.do_call(mFD, "Failed to do i2c smbus write byte");
}

std::byte i2c_device::smbus_read_byte(i2c_register reg)
{
    using namespace i2c;

    smbus_packet data;
    smbus_ioctl_data {
        smbus_cmd::read,
        reg.value(),
        smbus_tx_type::byte_data,
        &data,
    }.do_call(mFD, "Failed to do an i2c smbus byte register read");
    return data.byte;
}

void i2c_device::smbus_write_byte(i2c_register reg, std::byte value)
{
    using namespace i2c;

    smbus_packet data { value };
    smbus_ioctl_data {
        smbus_cmd::write,
        reg.value(),
        smbus_tx_type::byte_data,
        &data,
    }.do_call(mFD, "Failed to do an i2c smbus byte register write");
}

std::uint16_t i2c_device::smbus_read_word(i2c_register reg)
{
    using namespace i2c;

    smbus_packet data;
    smbus_ioctl_data {
        smbus_cmd::read,
        reg.value(),
        smbus_tx_type::word_data,
        &data
    }.do_call(mFD, "Failed to do an i2c smbus word register read");
    return data.word;
}
void i2c_device::smbus_write_word(i2c_register reg, std::uint16_t value)
{
    using namespace i2c;

    smbus_packet data;
    data.word = value;

    smbus_ioctl_data {
        smbus_cmd::write,
        reg.value(),
        smbus_tx_type::word_data,
        &data,
    }.do_call(mFD, "Failed to do an i2c smbus word register write");
}

std::uint16_t i2c_device::smbus_process_call(i2c_register call_id, std::uint16_t value)
{
    using namespace i2c;

    smbus_packet data;
    data.word = value;

    smbus_ioctl_data {
        smbus_cmd::write,
        call_id.value(),
        smbus_tx_type::proc_call,
        &data
    }.do_call(mFD, "Failed to do an i2c smbus process call");
    return data.word;
}


}
