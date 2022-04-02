// license:BSD-3-Clause
// copyright-holders:smf
/*********************************************************************

    i8251.h

    Intel 8251 Universal Synchronous/Asynchronous Receiver Transmitter code

*********************************************************************/

#ifndef MAME_MACHINE_I8251_H
#define MAME_MACHINE_I8251_H

#pragma once

#include "diserial.h"

class i8251_device :  public device_t,
	public device_serial_interface
{
public:
	// construction/destruction
	i8251_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration helpers
	auto txd_handler() { return m_txd_handler.bind(); }
	auto dtr_handler() { return m_dtr_handler.bind(); }
	auto rts_handler() { return m_rts_handler.bind(); }
	auto rxrdy_handler() { return m_rxrdy_handler.bind(); }
	auto txrdy_handler() { return m_txrdy_handler.bind(); }
	auto txempty_handler() { return m_txempty_handler.bind(); }
	auto syndet_handler() { return m_syndet_handler.bind(); }

	uint8_t data_r();
	void data_w(uint8_t data);
	uint8_t status_r();
	void control_w(uint8_t data);

	virtual uint8_t read(offs_t offset);
	virtual void write(offs_t offset, uint8_t data);

	DECLARE_WRITE_LINE_MEMBER( write_rxd );
	DECLARE_WRITE_LINE_MEMBER( write_cts );
	DECLARE_WRITE_LINE_MEMBER( write_dsr );
	DECLARE_WRITE_LINE_MEMBER( write_txc );
	DECLARE_WRITE_LINE_MEMBER( write_rxc );
	DECLARE_WRITE_LINE_MEMBER( write_syn );

	DECLARE_READ_LINE_MEMBER(txrdy_r);

protected:
	enum
	{
		I8251_STATUS_FRAMING_ERROR = 0x20,
		I8251_STATUS_OVERRUN_ERROR = 0x10,
		I8251_STATUS_PARITY_ERROR = 0x08,
		I8251_STATUS_TX_EMPTY = 0x04,
		I8251_STATUS_RX_READY = 0x02,
		I8251_STATUS_TX_READY = 0x01
	};

	i8251_device(
			const machine_config &mconfig,
			device_type type,
			const char *tag,
			device_t *owner,
			uint32_t clock);

	// device-level overrides
	virtual void device_resolve_objects() override;
	virtual void device_start() override;
	virtual void device_reset() override;

	void command_w(uint8_t data);
	void mode_w(uint8_t data);

	void receive_character(uint8_t ch);

	void update_rx_ready();
	void update_tx_ready();
	void update_tx_empty();
	void transmit_clock();
	void receive_clock();
	bool is_tx_enabled() const;
	void check_for_tx_start();
	void start_tx();


	enum
	{
		I8251_NEXT_COMMAND = 0,
		I8251_NEXT_MODE,
		I8251_NEXT_SYNC1,
		I8251_NEXT_SYNC2,
	};

private:
	devcb_write_line m_txd_handler;
	devcb_write_line m_dtr_handler;
	devcb_write_line m_rts_handler;
	devcb_write_line m_rxrdy_handler;
	devcb_write_line m_txrdy_handler;
	devcb_write_line m_txempty_handler;
	devcb_write_line m_syndet_handler;

	void sync1_rxc();
	void sync2_rxc();
	void update_syndet(bool voltage);
	bool calc_parity(u8 ch);

	/* flags controlling how i8251_control_w operates */
	uint8_t m_flags = 0;
	/* number of sync bytes programmed for sync mode (1 or 2) ; 0 = async mode */
	uint8_t m_sync_byte_count = 0;
	/* the sync bytes written */
	u8 m_sync1 = 0;
	u16 m_sync2 = 0;
	/* status of i8251 */
	uint8_t m_status = 0;
	uint8_t m_command = 0;
	/* mode byte - bit definitions depend on mode - e.g. synchronous, asynchronous */
	uint8_t m_mode_byte = 0;
	bool m_delayed_tx_en = false;

	bool m_cts;
	bool m_dsr;
	bool m_rxd;
	bool m_rxc;
	bool m_txc;
	int m_rxc_count = 0;
	int m_txc_count = 0;
	int m_br_factor = 0;

	/* data being received */
	uint8_t m_rx_data = 0;
		/* tx buffer */
	uint8_t m_tx_data = 0;
	void sync1_w(uint8_t data);
	void sync2_w(uint8_t data);
	uint8_t m_sync8 = 0;
	uint16_t m_sync16 = 0;
	// 1 = ext sync enabled via command
	bool m_syndet_pin = false;
	bool m_hunt_on = false;
	// 1 = ext syndet pin has been set high; 0 = hunt mode activated
	bool m_ext_syn_set = false;
	// count of rxd bits
	u8 m_rxd_bits = 0;
	u8 m_data_bits_count = 0;
};

class v5x_scu_device :  public i8251_device
{
public:
	// construction/destruction
	v5x_scu_device(const machine_config &mconfig,  const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read(offs_t offset) override;
	virtual void write(offs_t offset, uint8_t data) override;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	// TODO: currently unimplemented interrupt masking
	u8 simk_r() { return m_simk; }
	void simk_w(u8 data) { m_simk = data; }

private:
	u8 m_simk = 0;
};



// device type definition
DECLARE_DEVICE_TYPE(I8251,   i8251_device)
DECLARE_DEVICE_TYPE(V5X_SCU, v5x_scu_device)

#endif // MAME_MACHINE_I8251_H
