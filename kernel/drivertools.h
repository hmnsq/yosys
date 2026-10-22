/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2024  Jannis Harder <jix@yosyshq.com> <me@jix.one>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef DRIVERTOOLS_H
#define DRIVERTOOLS_H

#அந்தர்பாவ <string>
#அந்தர்பாவ <type_traits>

#அந்தர்பாவ "kernel/rtlil.h"
#அந்தர்பாவ "kernel/sigtools.h"
#அந்தர்பாவ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

கட்டமைப்பு DriveBit;

கட்டமைப்பு DriveChunkWire;
கட்டமைப்பு DriveChunkPort;
கட்டமைப்பு DriveChunkMarker;
கட்டமைப்பு DriveChunk;

கட்டமைப்பு DriveSpec;

std::string log_signal(DriveChunkWire மாறிலி &chunk);
std::string log_signal(DriveChunkPort மாறிலி &chunk);
std::string log_signal(DriveChunkMarker மாறிலி &chunk);
std::string log_signal(DriveChunk மாறிலி &chunk);
std::string log_signal(DriveSpec மாறிலி &chunk);

எண்ணிக்கை class DriveType : unsigned எழுத்து
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

கட்டமைப்பு DriveBitWire
{
	Wire *wire;
	முழு offset;

	DriveBitWire(Wire *wire, முழு offset) : wire(wire), offset(offset) {}

	bool operator==(மாறிலி DriveBitWire &other) மாறிலி
	{
		திரும்பு wire == other.wire && offset == other.offset;
	}

	bool operator<(மாறிலி DriveBitWire &other) மாறிலி
	{
		எனில் (wire != other.wire)
			திரும்பு wire->name < other.wire->name;
		திரும்பு offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;


	operator SigBit() மாறிலி
	{
		திரும்பு SigBit(wire, offset);
	}
};

கட்டமைப்பு DriveBitPort
{
	Cell *cell;
	IdString port;
	முழு offset;

	DriveBitPort(Cell *cell, IdString port, முழு offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(மாறிலி DriveBitPort &other) மாறிலி
	{
		திரும்பு cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(மாறிலி DriveBitPort &other) மாறிலி
	{
		எனில் (cell != other.cell)
			திரும்பு cell->name < other.cell->name;
		எனில் (port != other.port)
			திரும்பு port < other.port;
		திரும்பு offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;

};


கட்டமைப்பு DriveBitMarker
{
	முழு marker;
	முழு offset;

	DriveBitMarker(முழு marker, முழு offset) : marker(marker), offset(offset) {}

	bool operator==(மாறிலி DriveBitMarker &other) மாறிலி
	{
		திரும்பு marker == other.marker && offset == other.offset;
	}

	bool operator<(மாறிலி DriveBitMarker &other) மாறிலி
	{
		எனில் (marker != other.marker)
			திரும்பு marker < other.marker;
		திரும்பு offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;

};

கட்டமைப்பு DriveBitMultiple
{
தனிப்பட்ட:
	pool<DriveBit> multiple_;

பொது:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit மாறிலி &single);

	pool<DriveBit> மாறிலி &multiple() மாறிலி { திரும்பு multiple_; }

	வெறுமை merge(DriveBitMultiple மாறிலி &other)
	{
		க்கு (DriveBit மாறிலி &single : other.multiple_)
			merge(single);
	}

	வெறுமை merge(DriveBitMultiple &&other)
	{
		க்கு (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	வெறுமை merge(DriveBit மாறிலி &single);
	வெறுமை merge(DriveBit &&single);

	bool operator==(மாறிலி DriveBitMultiple &other) மாறிலி
	{
		திரும்பு multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;
};

கட்டமைப்பு DriveBit
{
தனிப்பட்ட:
	DriveType type_ = DriveType::NONE;
	ஒன்றிப்பு
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
பொது:
	DriveBit()  {}

	DriveBit(SigBit மாறிலி &bit);

	DriveBit(DriveBit மாறிலி &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire மாறிலி &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort மாறிலி &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker மாறிலி &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple மாறிலி &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	வெறுமை set_none()
	{
		தேர்வு (type_)
		{
			நிலை DriveType::NONE:
				நிறுத்து;
			நிலை DriveType::CONSTANT:
				நிறுத்து;
			நிலை DriveType::WIRE:
				wire_.~DriveBitWire();
				நிறுத்து;
			நிலை DriveType::PORT:
				port_.~DriveBitPort();
				நிறுத்து;
			நிலை DriveType::MARKER:
				marker_.~DriveBitMarker();
				நிறுத்து;
			நிலை DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				நிறுத்து;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit மாறிலி &other)
	{
		தேர்வு (other.type_)
		{
			நிலை DriveType::NONE:
				set_none();
				நிறுத்து;
			நிலை DriveType::CONSTANT:
				*this = other.constant_;
				நிறுத்து;
			நிலை DriveType::WIRE:
				*this = other.wire_;
				நிறுத்து;
			நிலை DriveType::PORT:
				*this = other.port_;
				நிறுத்து;
			நிலை DriveType::MARKER:
				*this = other.marker_;
				நிறுத்து;
			நிலை DriveType::MULTIPLE:
				*this = other.multiple_;
				நிறுத்து;
		}
		திரும்பு *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		தேர்வு (other.type_)
		{
			நிலை DriveType::NONE:
				set_none();
				நிறுத்து;
			நிலை DriveType::CONSTANT:
				*this = std::move(other.constant_);
				நிறுத்து;
			நிலை DriveType::WIRE:
				*this = std::move(other.wire_);
				நிறுத்து;
			நிலை DriveType::PORT:
				*this = std::move(other.port_);
				நிறுத்து;
			நிலை DriveType::MARKER:
				*this = std::move(other.marker_);
				நிறுத்து;
			நிலை DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				நிறுத்து;
		}
		திரும்பு *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		திரும்பு *this;
	}

	DriveBit &operator=(DriveBitWire மாறிலி &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		திரும்பு *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		திரும்பு *this;
	}

	DriveBit &operator=(DriveBitPort மாறிலி &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		திரும்பு *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		திரும்பு *this;
	}

	DriveBit &operator=(DriveBitMarker மாறிலி &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		திரும்பு *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		திரும்பு *this;
	}

	DriveBit &operator=(DriveBitMultiple மாறிலி &multiple)
	{
		set_none();
		எனில் (multiple.multiple().empty())
			திரும்பு *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		திரும்பு *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		எனில் (multiple.multiple().empty())
			திரும்பு *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		திரும்பு *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;

	bool operator==(மாறிலி DriveBit &other) மாறிலி
	{
		எனில் (type_ != other.type_)
			திரும்பு false;

		தேர்வு (type_)
		{
			நிலை DriveType::NONE:
				திரும்பு true;
			நிலை DriveType::CONSTANT:
				திரும்பு constant_ == other.constant_;
			நிலை DriveType::WIRE:
				திரும்பு wire_ == other.wire_;
			நிலை DriveType::PORT:
				திரும்பு port_ == other.port_;
			நிலை DriveType::MARKER:
				திரும்பு marker_ == other.marker_;
			நிலை DriveType::MULTIPLE:
				திரும்பு multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(மாறிலி DriveBit &other) மாறிலி
	{
		திரும்பு !(*this == other);
	}

	bool operator<(மாறிலி DriveBit &other) மாறிலி
	{
		எனில் (type_ != other.type_)
			திரும்பு type_ < other.type_;
		தேர்வு (type_)
		{
			நிலை DriveType::NONE:
				திரும்பு false;
			நிலை DriveType::CONSTANT:
				திரும்பு constant_ < other.constant_;
			நிலை DriveType::WIRE:
				திரும்பு wire_ < other.wire_;
			நிலை DriveType::PORT:
				திரும்பு port_ < other.port_;
			நிலை DriveType::MARKER:
				திரும்பு marker_ < other.marker_;
			நிலை DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() மாறிலி { திரும்பு type_; }

	bool is_none() மாறிலி { திரும்பு type_ == DriveType::NONE; }
	bool is_constant() மாறிலி { திரும்பு type_ == DriveType::CONSTANT; }
	bool is_wire() மாறிலி { திரும்பு type_ == DriveType::WIRE; }
	bool is_port() மாறிலி { திரும்பு type_ == DriveType::PORT; }
	bool is_marker() மாறிலி { திரும்பு type_ == DriveType::MARKER; }
	bool is_multiple() மாறிலி { திரும்பு type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); திரும்பு constant_; }
	State மாறிலி &constant() மாறிலி { log_assert(is_constant()); திரும்பு constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); திரும்பு wire_; }
	DriveBitWire மாறிலி &wire() மாறிலி { log_assert(is_wire()); திரும்பு wire_; }
	DriveBitPort &port() { log_assert(is_port()); திரும்பு port_; }
	DriveBitPort மாறிலி &port() மாறிலி { log_assert(is_port()); திரும்பு port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); திரும்பு marker_; }
	DriveBitMarker மாறிலி &marker() மாறிலி { log_assert(is_marker()); திரும்பு marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); திரும்பு multiple_; }
	DriveBitMultiple மாறிலி &multiple() மாறிலி { log_assert(is_multiple()); திரும்பு multiple_; }

	வெறுமை merge(DriveBit மாறிலி &other);

};

உள்வரி DriveBitMultiple::DriveBitMultiple() {}
உள்வரி DriveBitMultiple::DriveBitMultiple(DriveBit மாறிலி &single)
{
	multiple_.emplace(single);
}

கட்டமைப்பு DriveChunkWire
{
	Wire *wire;
	முழு offset;
	முழு width;

	DriveChunkWire(Wire *wire, முழு offset, முழு width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire மாறிலி &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	முழு size() மாறிலி { திரும்பு width; }

	DriveBitWire operator[](முழு i) மாறிலி
	{
		log_assert(i >= 0 && i < width);
		திரும்பு DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire மாறிலி &bit) மாறிலி;
	bool try_append(DriveBitWire மாறிலி &bit);
	bool try_append(DriveChunkWire மாறிலி &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() மாறிலி { திரும்பு offset == 0 && width == wire->width; }

	bool operator==(மாறிலி DriveChunkWire &other) மாறிலி
	{
		திரும்பு wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(மாறிலி DriveChunkWire &other) மாறிலி
	{
		எனில் (wire != other.wire)
			திரும்பு wire->name < other.wire->name;
		எனில் (width != other.width)
			திரும்பு width < other.width;
		திரும்பு offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;

	explicit operator SigChunk() மாறிலி
	{
		திரும்பு SigChunk(wire, offset, width);
	}
};

கட்டமைப்பு DriveChunkPort
{
	Cell *cell;
	IdString port;
	முழு offset;
	முழு width;

	DriveChunkPort(Cell *cell, IdString port, முழு offset, முழு width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> மாறிலி &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort மாறிலி &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	முழு size() மாறிலி { திரும்பு width; }

	DriveBitPort operator[](முழு i) மாறிலி
	{
		log_assert(i >= 0 && i < width);
		திரும்பு DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort மாறிலி &bit) மாறிலி;
	bool try_append(DriveBitPort மாறிலி &bit);
	bool try_append(DriveChunkPort மாறிலி &chunk);

	// Whether this chunk is a whole port
	bool is_whole() மாறிலி
	{
		திரும்பு offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(மாறிலி DriveChunkPort &other) மாறிலி
	{
		திரும்பு cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(மாறிலி DriveChunkPort &other) மாறிலி
	{
		எனில் (cell != other.cell)
			திரும்பு cell->name < other.cell->name;
		எனில் (port != other.port)
			திரும்பு port < other.port;
		எனில் (width != other.width)
			திரும்பு width < other.width;
		திரும்பு offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;
};


கட்டமைப்பு DriveChunkMarker
{
	முழு marker;
	முழு offset;
	முழு width;

	DriveChunkMarker(முழு marker, முழு offset, முழு width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker மாறிலி &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	முழு size() மாறிலி { திரும்பு width; }

	DriveBitMarker operator[](முழு i) மாறிலி
	{
		log_assert(i >= 0 && i < width);
		திரும்பு DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker மாறிலி &bit) மாறிலி;
	bool try_append(DriveBitMarker மாறிலி &bit);
	bool try_append(DriveChunkMarker மாறிலி &chunk);

	bool operator==(மாறிலி DriveChunkMarker &other) மாறிலி
	{
		திரும்பு marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(மாறிலி DriveChunkMarker &other) மாறிலி
	{
		எனில் (marker != other.marker)
			திரும்பு marker < other.marker;
		எனில் (width != other.width)
			திரும்பு width < other.width;
		திரும்பு offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;
};

கட்டமைப்பு DriveChunkMultiple
{
தனிப்பட்ட:
	mutable pool<DriveChunk> multiple_;
	முழு width_;

பொது:
	pool<DriveChunk> மாறிலி &multiple() மாறிலி { திரும்பு multiple_; }

	DriveChunkMultiple(DriveBitMultiple மாறிலி &bit);

	முழு size() மாறிலி { திரும்பு width_; }

	DriveBitMultiple operator[](முழு i) மாறிலி;

	bool can_append(DriveBitMultiple மாறிலி &bit) மாறிலி;

	bool try_append(DriveBitMultiple மாறிலி &bit);


	bool can_append(DriveChunkMultiple மாறிலி &bit) மாறிலி;

	bool try_append(DriveChunkMultiple மாறிலி &bit);

	bool operator==(மாறிலி DriveChunkMultiple &other) மாறிலி
	{
		திரும்பு width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(மாறிலி DriveChunkMultiple &other) மாறிலி
	{
		எனில் (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		திரும்பு false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;
};

கட்டமைப்பு DriveChunk
{
தனிப்பட்ட:
	DriveType type_ = DriveType::NONE;
	ஒன்றிப்பு
	{
		முழு none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

பொது:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk மாறிலி &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit மாறிலி &other) { *this = other; }

	DriveChunk(Const மாறிலி &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire மாறிலி &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort மாறிலி &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker மாறிலி &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple மாறிலி &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](முழு i) மாறிலி
	{
		தேர்வு (type_)
		{
			நிலை DriveType::NONE:
				திரும்பு DriveBit();
			நிலை DriveType::CONSTANT:
				திரும்பு constant_[i];
			நிலை DriveType::WIRE:
				திரும்பு wire_[i];
			நிலை DriveType::PORT:
				திரும்பு port_[i];
			நிலை DriveType::MARKER:
				திரும்பு marker_[i];
			நிலை DriveType::MULTIPLE:
				திரும்பு multiple_[i];
		}
		log_abort();
	}

	வெறுமை set_none(முழு width = 0)
	{
		தேர்வு (type_)
		{
			நிலை DriveType::NONE:
				none_ = width;
				நிறுத்து;
			நிலை DriveType::CONSTANT:
				constant_.~Const();
				நிறுத்து;
			நிலை DriveType::WIRE:
				wire_.~DriveChunkWire();
				நிறுத்து;
			நிலை DriveType::PORT:
				port_.~DriveChunkPort();
				நிறுத்து;
			நிலை DriveType::MARKER:
				marker_.~DriveChunkMarker();
				நிறுத்து;
			நிலை DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				நிறுத்து;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk மாறிலி &other)
	{
		தேர்வு (other.type_)
		{
			நிலை DriveType::NONE:
				set_none(other.none_);
				நிறுத்து;
			நிலை DriveType::CONSTANT:
				*this = other.constant_;
				நிறுத்து;
			நிலை DriveType::WIRE:
				*this = other.wire_;
				நிறுத்து;
			நிலை DriveType::PORT:
				*this = other.port_;
				நிறுத்து;
			நிலை DriveType::MARKER:
				*this = other.marker_;
				நிறுத்து;
			நிலை DriveType::MULTIPLE:
				*this = other.multiple_;
				நிறுத்து;
		}
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		தேர்வு (other.type_)
		{
			நிலை DriveType::NONE:
				set_none(other.none_);
				நிறுத்து;
			நிலை DriveType::CONSTANT:
				*this = std::move(other.constant_);
				நிறுத்து;
			நிலை DriveType::WIRE:
				*this = std::move(other.wire_);
				நிறுத்து;
			நிலை DriveType::PORT:
				*this = std::move(other.port_);
				நிறுத்து;
			நிலை DriveType::MARKER:
				*this = std::move(other.marker_);
				நிறுத்து;
			நிலை DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				நிறுத்து;
		}
		திரும்பு *this;
	}

	DriveChunk &operator=(Const மாறிலி &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		திரும்பு *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveChunkWire மாறிலி &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveChunkPort மாறிலி &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveChunkMarker மாறிலி &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveChunkMultiple மாறிலி &multiple)
	{
		set_none(multiple.size());
		எனில் (multiple.multiple().empty())
			திரும்பு *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		எனில் (multiple.multiple().empty())
			திரும்பு *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		திரும்பு *this;
	}

	DriveChunk &operator=(DriveBit மாறிலி &other)
	{
		தேர்வு (other.type())
		{
			நிலை DriveType::NONE:
				set_none(1);
				நிறுத்து;
			நிலை DriveType::CONSTANT:
				*this = Const(other.constant());
				நிறுத்து;
			நிலை DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				நிறுத்து;
			நிலை DriveType::PORT:
				*this = DriveChunkPort(other.port());
				நிறுத்து;
			நிலை DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				நிறுத்து;
			நிலை DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				நிறுத்து;
		}
		திரும்பு *this;
	}

	bool can_append(DriveBit மாறிலி &bit) மாறிலி;
	bool try_append(DriveBit மாறிலி &bit);
	bool try_append(DriveChunk மாறிலி &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;

	bool operator==(மாறிலி DriveChunk &other) மாறிலி
	{
		எனில் (type_ != other.type_)
			திரும்பு false;

		தேர்வு (type_)
		{
			நிலை DriveType::NONE:
				திரும்பு true;
			நிலை DriveType::CONSTANT:
				திரும்பு constant_ == other.constant_;
			நிலை DriveType::WIRE:
				திரும்பு wire_ == other.wire_;
			நிலை DriveType::PORT:
				திரும்பு port_ == other.port_;
			நிலை DriveType::MARKER:
				திரும்பு marker_ == other.marker_;
			நிலை DriveType::MULTIPLE:
				திரும்பு multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(மாறிலி DriveChunk &other) மாறிலி
	{
		திரும்பு !(*this == other);
	}

	bool operator<(மாறிலி DriveChunk &other) மாறிலி
	{
		எனில் (type_ != other.type_)
			திரும்பு type_ < other.type_;
		தேர்வு (type_)
		{
			நிலை DriveType::NONE:
				திரும்பு false;
			நிலை DriveType::CONSTANT:
				திரும்பு constant_ < other.constant_;
			நிலை DriveType::WIRE:
				திரும்பு wire_ < other.wire_;
			நிலை DriveType::PORT:
				திரும்பு port_ < other.port_;
			நிலை DriveType::MARKER:
				திரும்பு marker_ < other.marker_;
			நிலை DriveType::MULTIPLE:
				திரும்பு multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() மாறிலி { திரும்பு type_; }

	bool is_none() மாறிலி { திரும்பு type_ == DriveType::NONE; }
	bool is_constant() மாறிலி { திரும்பு type_ == DriveType::CONSTANT; }
	bool is_wire() மாறிலி { திரும்பு type_ == DriveType::WIRE; }
	bool is_port() மாறிலி { திரும்பு type_ == DriveType::PORT; }
	bool is_marker() மாறிலி { திரும்பு type_ == DriveType::MARKER; }
	bool is_multiple() மாறிலி { திரும்பு type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); திரும்பு constant_; }
	Const மாறிலி &constant() மாறிலி { log_assert(is_constant()); திரும்பு constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); திரும்பு wire_; }
	DriveChunkWire மாறிலி &wire() மாறிலி { log_assert(is_wire()); திரும்பு wire_; }
	DriveChunkPort &port() { log_assert(is_port()); திரும்பு port_; }
	DriveChunkPort மாறிலி &port() மாறிலி { log_assert(is_port()); திரும்பு port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); திரும்பு marker_; }
	DriveChunkMarker மாறிலி &marker() மாறிலி { log_assert(is_marker()); திரும்பு marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); திரும்பு multiple_; }
	DriveChunkMultiple மாறிலி &multiple() மாறிலி { log_assert(is_multiple()); திரும்பு multiple_; }


	முழு size() மாறிலி
	{
		தேர்வு (type_)
		{
			நிலை DriveType::NONE:
				திரும்பு none_;
			நிலை DriveType::CONSTANT:
				திரும்பு constant_.size();
			நிலை DriveType::WIRE:
				திரும்பு wire_.size();
			நிலை DriveType::PORT:
				திரும்பு port_.size();
			நிலை DriveType::MARKER:
				திரும்பு marker_.size();
			நிலை DriveType::MULTIPLE:
				திரும்பு multiple_.size();
		}
		log_abort();
	}
};

உள்வரி DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple மாறிலி &bit)
	: width_(1)
{
	க்கு (auto மாறிலி &bit : bit.multiple())
		multiple_.emplace(bit);
}

கட்டமைப்பு DriveSpec
{
தனிப்பட்ட:
	முழு width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned முழு hash_ = 0;
பொது:

	உள்வரி bool packed() மாறிலி {
		திரும்பு bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk மாறிலி &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire மாறிலி &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort மாறிலி &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker மாறிலி &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple மாறிலி &chunk) { *this = chunk; }

	DriveSpec(DriveBit மாறிலி &bit) { *this = bit; }
	DriveSpec(DriveBitWire மாறிலி &bit) { *this = bit; }
	DriveSpec(DriveBitPort மாறிலி &bit) { *this = bit; }
	DriveSpec(DriveBitMarker மாறிலி &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple மாறிலி &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> மாறிலி &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> மாறிலி &bits)
	{
		க்கு (auto மாறிலி &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec மாறிலி &sig)
	{
		// TODO: converting one chunk at a time would be faster
		க்கு (auto மாறிலி &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> மாறிலி &chunks() மாறிலி { pack(); திரும்பு chunks_; }
	std::vector<DriveBit> மாறிலி &bits() மாறிலி { unpack(); திரும்பு bits_; }

	முழு size() மாறிலி { திரும்பு width_; }

	வெறுமை append(DriveBit மாறிலி &bit);

	வெறுமை append(DriveChunk மாறிலி &chunk);

	வெறுமை pack() மாறிலி;

	வெறுமை unpack() மாறிலி;

	DriveBit &operator[](முழு index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		திரும்பு bits_[index];
	}

	மாறிலி DriveBit &operator[](முழு index) மாறிலி
	{
		log_assert(index >= 0 && index < size());
		unpack();
		திரும்பு bits_[index];
	}

	வெறுமை clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk மாறிலி &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		திரும்பு *this;
	}

	DriveSpec &operator=(DriveChunkWire மாறிலி &chunk) { திரும்பு *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort மாறிலி &chunk) { திரும்பு *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker மாறிலி &chunk) { திரும்பு *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple மாறிலி &chunk) { திரும்பு *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit மாறிலி &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		திரும்பு *this;
	}

	DriveSpec &operator=(DriveBitWire மாறிலி &bit) { திரும்பு *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort மாறிலி &bit) { திரும்பு *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker மாறிலி &bit) { திரும்பு *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple மாறிலி &bit) { திரும்பு *this = DriveBit(bit); }

	வெறுமை updhash() மாறிலி {
		எனில் (hash_ != 0)
			திரும்பு;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;

	bool operator==(DriveSpec மாறிலி &other) மாறிலி {
		updhash();
		other.updhash();
		எனில் (size() != other.size() || hash_ != other.hash_)
			திரும்பு false;
		திரும்பு chunks() == other.chunks();
	}

தனிப்பட்ட:
	வெறுமை compute_width();
};



கட்டமைப்பு DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

தனிப்பட்ட:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	கட்டமைப்பு DriveBitId
	{
		முழு id = -1;

		DriveBitId() {};

		DriveBitId(முழு id) : id(id) { }

		bool operator==(மாறிலி DriveBitId &other) மாறிலி { திரும்பு id == other.id; }
		bool operator!=(மாறிலி DriveBitId &other) மாறிலி { திரும்பு id != other.id; }
		bool operator<(மாறிலி DriveBitId &other) மாறிலி { திரும்பு id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	கட்டமைப்பு DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		வெறுமை add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId எடு_edge(DriveBitId src);
		வெறுமை clear(DriveBitId src);
		bool contains(DriveBitId src);
		முழு மொத்தம்(DriveBitId src);

		DriveBitId at(DriveBitId src, முழு index);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, DriveBitId> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, DriveBitId> port_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<DriveBitId, DriveBit> drive_bits;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<DriveBitId, DriveBit> isolated_drive_bits;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	முழு next_offset = 1 + (முழு)State::Sm;

	// Union-Find over directly connected bits that share the same single
	// driver or are undriven. We never merge connections between drivers
	// and/or kept wires.
	mfp<DriveBitId> same_driver;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally unambiguous (one side only drives or requires a driven value).
	DriveBitGraph connected_drivers;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally ambiguous. Every such ambiguous connection is also present in
	// the reverse direction and has to be resolved when querying drivers.
	DriveBitGraph connected_undirected;

	// Subset of `connected_undirected` for caching the resolved driving
	// direction. In case multiple drivers are present this can still contain
	// both orientations of a single connection, but for a single driver only
	// one will be present.
	DriveBitGraph connected_oriented;

	// Stores for which bits we already resolved the orientation (cached in
	// `connected_oriented`).
	pool<DriveBitId> oriented_present;


	எண்ணிக்கை class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit மாறிலி &bit);
	DriveBitId id_from_drive_bit(DriveBit மாறிலி &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	வெறுமை connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	வெறுமை connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	வெறுமை connect_undirected(DriveBitId a_id, DriveBitId b_id);

பொது:

	வெறுமை add(Module *module);

	// Add a single bit connection to the driver map.
	வெறுமை add(DriveBit மாறிலி &a, DriveBit மாறிலி &b);

	template<typename T>
	நிலையான constexpr bool is_sig_type() {
		திரும்பு
			std::is_same<T, SigSpec>::மதிப்பு ||
			std::is_same<T, SigChunk>::மதிப்பு ||
			std::is_same<T, DriveSpec>::மதிப்பு ||
			std::is_same<T, DriveChunk>::மதிப்பு ||
			std::is_same<T, DriveChunkPort>::மதிப்பு ||
			std::is_same<T, DriveChunkWire>::மதிப்பு ||
			std::is_same<T, Const>::மதிப்பு;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T மாறிலி &a, U மாறிலி &b)
	{
		log_assert(a.size() == b.size());
		க்கு (முழு i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	வெறுமை add(SigSpec மாறிலி &a, SigSpec மாறிலி &b);

தனிப்பட்ட:
	வெறுமை add_port(Cell *cell, IdString port, SigSpec மாறிலி &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, முழு> orient_undirected_distance;

	வெறுமை orient_undirected(DriveBitId id);

பொது:
	DriveBit operator()(DriveBit மாறிலி &bit);

	DriveSpec operator()(DriveSpec spec);

தனிப்பட்ட:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		திரும்பு wire->has_attribute(ID(keep));
	}
};

உள்வரி Hasher DriveBitWire::hash_into(Hasher h) மாறிலி
{
	h.eat(wire->name);
	h.eat(offset);
	திரும்பு h;
}

உள்வரி Hasher DriveBitPort::hash_into(Hasher h) மாறிலி
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	திரும்பு h;
}

உள்வரி Hasher DriveBitMarker::hash_into(Hasher h) மாறிலி
{
	h.eat(marker);
	h.eat(offset);
	திரும்பு h;
}

உள்வரி Hasher DriveBitMultiple::hash_into(Hasher h) மாறிலி
{
	h.eat(multiple_);
	திரும்பு h;
}

உள்வரி Hasher DriveBit::hash_into(Hasher h) மாறிலி
{
	தேர்வு (type_) {
	நிலை DriveType::NONE:
		h.eat(0);
		நிறுத்து;
	நிலை DriveType::CONSTANT:
		h.eat(constant_);
		நிறுத்து;
	நிலை DriveType::WIRE:
		h.eat(wire_);
		நிறுத்து;
	நிலை DriveType::PORT:
		h.eat(port_);
		நிறுத்து;
	நிலை DriveType::MARKER:
		h.eat(marker_);
		நிறுத்து;
	நிலை DriveType::MULTIPLE:
		h.eat(multiple_);
		நிறுத்து;
	}
	h.eat(type_);
	திரும்பு h;
}

உள்வரி Hasher DriveChunkWire::hash_into(Hasher h) மாறிலி
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	திரும்பு h;
}

உள்வரி Hasher DriveChunkPort::hash_into(Hasher h) மாறிலி
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	திரும்பு h;
}

உள்வரி Hasher DriveChunkMarker::hash_into(Hasher h) மாறிலி
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	திரும்பு h;
}

உள்வரி Hasher DriveChunkMultiple::hash_into(Hasher h) மாறிலி
{
	h.eat(width_);
	h.eat(multiple_);
	திரும்பு h;
}

உள்வரி Hasher DriveChunk::hash_into(Hasher h) மாறிலி
{
	தேர்வு (type_) {
	நிலை DriveType::NONE:
		h.eat(0);
		நிறுத்து;
	நிலை DriveType::CONSTANT:
		h.eat(constant_);
		நிறுத்து;
	நிலை DriveType::WIRE:
		h.eat(wire_);
		நிறுத்து;
	நிலை DriveType::PORT:
		h.eat(port_);
		நிறுத்து;
	நிலை DriveType::MARKER:
		h.eat(marker_);
		நிறுத்து;
	நிலை DriveType::MULTIPLE:
		h.eat(multiple_);
		நிறுத்து;
	}
	h.eat(type_);
	திரும்பு h;
}

உள்வரி Hasher DriveSpec::hash_into(Hasher h) மாறிலி
{
	updhash();
	h.eat(hash_);
	திரும்பு h;
}

உள்வரி Hasher DriverMap::DriveBitId::hash_into(Hasher h) மாறிலி
{
	h.eat(id);
	திரும்பு h;
}

YOSYS_NAMESPACE_END

#endif
