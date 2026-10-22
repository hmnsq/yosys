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

#অন্তর্ভাব <string>
#অন্তর্ভাব <type_traits>

#অন্তর্ভাব "kernel/rtlil.h"
#অন্তর্ভাব "kernel/sigtools.h"
#অন্তর্ভাব "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

সংরচনা DriveBit;

সংরচনা DriveChunkWire;
সংরচনা DriveChunkPort;
সংরচনা DriveChunkMarker;
সংরচনা DriveChunk;

সংরচনা DriveSpec;

std::string log_signal(DriveChunkWire ধ্রুব &chunk);
std::string log_signal(DriveChunkPort ধ্রুব &chunk);
std::string log_signal(DriveChunkMarker ধ্রুব &chunk);
std::string log_signal(DriveChunk ধ্রুব &chunk);
std::string log_signal(DriveSpec ধ্রুব &chunk);

গণনা class DriveType : unsigned বর্ণ
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

সংরচনা DriveBitWire
{
	Wire *wire;
	পূর্ণাঙ্ক offset;

	DriveBitWire(Wire *wire, পূর্ণাঙ্ক offset) : wire(wire), offset(offset) {}

	bool operator==(ধ্রুব DriveBitWire &other) ধ্রুব
	{
		প্রত্যাগম wire == other.wire && offset == other.offset;
	}

	bool operator<(ধ্রুব DriveBitWire &other) ধ্রুব
	{
		যদি (wire != other.wire)
			প্রত্যাগম wire->name < other.wire->name;
		প্রত্যাগম offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;


	operator SigBit() ধ্রুব
	{
		প্রত্যাগম SigBit(wire, offset);
	}
};

সংরচনা DriveBitPort
{
	Cell *cell;
	IdString port;
	পূর্ণাঙ্ক offset;

	DriveBitPort(Cell *cell, IdString port, পূর্ণাঙ্ক offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(ধ্রুব DriveBitPort &other) ধ্রুব
	{
		প্রত্যাগম cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(ধ্রুব DriveBitPort &other) ধ্রুব
	{
		যদি (cell != other.cell)
			প্রত্যাগম cell->name < other.cell->name;
		যদি (port != other.port)
			প্রত্যাগম port < other.port;
		প্রত্যাগম offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;

};


সংরচনা DriveBitMarker
{
	পূর্ণাঙ্ক marker;
	পূর্ণাঙ্ক offset;

	DriveBitMarker(পূর্ণাঙ্ক marker, পূর্ণাঙ্ক offset) : marker(marker), offset(offset) {}

	bool operator==(ধ্রুব DriveBitMarker &other) ধ্রুব
	{
		প্রত্যাগম marker == other.marker && offset == other.offset;
	}

	bool operator<(ধ্রুব DriveBitMarker &other) ধ্রুব
	{
		যদি (marker != other.marker)
			প্রত্যাগম marker < other.marker;
		প্রত্যাগম offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;

};

সংরচনা DriveBitMultiple
{
গুপ্ত:
	pool<DriveBit> multiple_;

সার্বজনিক:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit ধ্রুব &single);

	pool<DriveBit> ধ্রুব &multiple() ধ্রুব { প্রত্যাগম multiple_; }

	শূন্য merge(DriveBitMultiple ধ্রুব &other)
	{
		প্রতি (DriveBit ধ্রুব &single : other.multiple_)
			merge(single);
	}

	শূন্য merge(DriveBitMultiple &&other)
	{
		প্রতি (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	শূন্য merge(DriveBit ধ্রুব &single);
	শূন্য merge(DriveBit &&single);

	bool operator==(ধ্রুব DriveBitMultiple &other) ধ্রুব
	{
		প্রত্যাগম multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;
};

সংরচনা DriveBit
{
গুপ্ত:
	DriveType type_ = DriveType::NONE;
	সঙ্ঘ
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
সার্বজনিক:
	DriveBit()  {}

	DriveBit(SigBit ধ্রুব &bit);

	DriveBit(DriveBit ধ্রুব &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire ধ্রুব &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort ধ্রুব &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker ধ্রুব &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple ধ্রুব &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	শূন্য set_none()
	{
		বিকল্প (type_)
		{
			পক্ষ DriveType::NONE:
				ভঙ্গ;
			পক্ষ DriveType::CONSTANT:
				ভঙ্গ;
			পক্ষ DriveType::WIRE:
				wire_.~DriveBitWire();
				ভঙ্গ;
			পক্ষ DriveType::PORT:
				port_.~DriveBitPort();
				ভঙ্গ;
			পক্ষ DriveType::MARKER:
				marker_.~DriveBitMarker();
				ভঙ্গ;
			পক্ষ DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				ভঙ্গ;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit ধ্রুব &other)
	{
		বিকল্প (other.type_)
		{
			পক্ষ DriveType::NONE:
				set_none();
				ভঙ্গ;
			পক্ষ DriveType::CONSTANT:
				*this = other.constant_;
				ভঙ্গ;
			পক্ষ DriveType::WIRE:
				*this = other.wire_;
				ভঙ্গ;
			পক্ষ DriveType::PORT:
				*this = other.port_;
				ভঙ্গ;
			পক্ষ DriveType::MARKER:
				*this = other.marker_;
				ভঙ্গ;
			পক্ষ DriveType::MULTIPLE:
				*this = other.multiple_;
				ভঙ্গ;
		}
		প্রত্যাগম *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		বিকল্প (other.type_)
		{
			পক্ষ DriveType::NONE:
				set_none();
				ভঙ্গ;
			পক্ষ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ভঙ্গ;
			পক্ষ DriveType::WIRE:
				*this = std::move(other.wire_);
				ভঙ্গ;
			পক্ষ DriveType::PORT:
				*this = std::move(other.port_);
				ভঙ্গ;
			পক্ষ DriveType::MARKER:
				*this = std::move(other.marker_);
				ভঙ্গ;
			পক্ষ DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				ভঙ্গ;
		}
		প্রত্যাগম *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		প্রত্যাগম *this;
	}

	DriveBit &operator=(DriveBitWire ধ্রুব &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		প্রত্যাগম *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		প্রত্যাগম *this;
	}

	DriveBit &operator=(DriveBitPort ধ্রুব &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		প্রত্যাগম *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		প্রত্যাগম *this;
	}

	DriveBit &operator=(DriveBitMarker ধ্রুব &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		প্রত্যাগম *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		প্রত্যাগম *this;
	}

	DriveBit &operator=(DriveBitMultiple ধ্রুব &multiple)
	{
		set_none();
		যদি (multiple.multiple().empty())
			প্রত্যাগম *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		প্রত্যাগম *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		যদি (multiple.multiple().empty())
			প্রত্যাগম *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		প্রত্যাগম *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;

	bool operator==(ধ্রুব DriveBit &other) ধ্রুব
	{
		যদি (type_ != other.type_)
			প্রত্যাগম false;

		বিকল্প (type_)
		{
			পক্ষ DriveType::NONE:
				প্রত্যাগম true;
			পক্ষ DriveType::CONSTANT:
				প্রত্যাগম constant_ == other.constant_;
			পক্ষ DriveType::WIRE:
				প্রত্যাগম wire_ == other.wire_;
			পক্ষ DriveType::PORT:
				প্রত্যাগম port_ == other.port_;
			পক্ষ DriveType::MARKER:
				প্রত্যাগম marker_ == other.marker_;
			পক্ষ DriveType::MULTIPLE:
				প্রত্যাগম multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ধ্রুব DriveBit &other) ধ্রুব
	{
		প্রত্যাগম !(*this == other);
	}

	bool operator<(ধ্রুব DriveBit &other) ধ্রুব
	{
		যদি (type_ != other.type_)
			প্রত্যাগম type_ < other.type_;
		বিকল্প (type_)
		{
			পক্ষ DriveType::NONE:
				প্রত্যাগম false;
			পক্ষ DriveType::CONSTANT:
				প্রত্যাগম constant_ < other.constant_;
			পক্ষ DriveType::WIRE:
				প্রত্যাগম wire_ < other.wire_;
			পক্ষ DriveType::PORT:
				প্রত্যাগম port_ < other.port_;
			পক্ষ DriveType::MARKER:
				প্রত্যাগম marker_ < other.marker_;
			পক্ষ DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ধ্রুব { প্রত্যাগম type_; }

	bool is_none() ধ্রুব { প্রত্যাগম type_ == DriveType::NONE; }
	bool is_constant() ধ্রুব { প্রত্যাগম type_ == DriveType::CONSTANT; }
	bool is_wire() ধ্রুব { প্রত্যাগম type_ == DriveType::WIRE; }
	bool is_port() ধ্রুব { প্রত্যাগম type_ == DriveType::PORT; }
	bool is_marker() ধ্রুব { প্রত্যাগম type_ == DriveType::MARKER; }
	bool is_multiple() ধ্রুব { প্রত্যাগম type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); প্রত্যাগম constant_; }
	State ধ্রুব &constant() ধ্রুব { log_assert(is_constant()); প্রত্যাগম constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); প্রত্যাগম wire_; }
	DriveBitWire ধ্রুব &wire() ধ্রুব { log_assert(is_wire()); প্রত্যাগম wire_; }
	DriveBitPort &port() { log_assert(is_port()); প্রত্যাগম port_; }
	DriveBitPort ধ্রুব &port() ধ্রুব { log_assert(is_port()); প্রত্যাগম port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); প্রত্যাগম marker_; }
	DriveBitMarker ধ্রুব &marker() ধ্রুব { log_assert(is_marker()); প্রত্যাগম marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); প্রত্যাগম multiple_; }
	DriveBitMultiple ধ্রুব &multiple() ধ্রুব { log_assert(is_multiple()); প্রত্যাগম multiple_; }

	শূন্য merge(DriveBit ধ্রুব &other);

};

অন্তঃস্থ DriveBitMultiple::DriveBitMultiple() {}
অন্তঃস্থ DriveBitMultiple::DriveBitMultiple(DriveBit ধ্রুব &single)
{
	multiple_.emplace(single);
}

সংরচনা DriveChunkWire
{
	Wire *wire;
	পূর্ণাঙ্ক offset;
	পূর্ণাঙ্ক width;

	DriveChunkWire(Wire *wire, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire ধ্রুব &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	পূর্ণাঙ্ক size() ধ্রুব { প্রত্যাগম width; }

	DriveBitWire operator[](পূর্ণাঙ্ক i) ধ্রুব
	{
		log_assert(i >= 0 && i < width);
		প্রত্যাগম DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire ধ্রুব &bit) ধ্রুব;
	bool try_append(DriveBitWire ধ্রুব &bit);
	bool try_append(DriveChunkWire ধ্রুব &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() ধ্রুব { প্রত্যাগম offset == 0 && width == wire->width; }

	bool operator==(ধ্রুব DriveChunkWire &other) ধ্রুব
	{
		প্রত্যাগম wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(ধ্রুব DriveChunkWire &other) ধ্রুব
	{
		যদি (wire != other.wire)
			প্রত্যাগম wire->name < other.wire->name;
		যদি (width != other.width)
			প্রত্যাগম width < other.width;
		প্রত্যাগম offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;

	explicit operator SigChunk() ধ্রুব
	{
		প্রত্যাগম SigChunk(wire, offset, width);
	}
};

সংরচনা DriveChunkPort
{
	Cell *cell;
	IdString port;
	পূর্ণাঙ্ক offset;
	পূর্ণাঙ্ক width;

	DriveChunkPort(Cell *cell, IdString port, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> ধ্রুব &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort ধ্রুব &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	পূর্ণাঙ্ক size() ধ্রুব { প্রত্যাগম width; }

	DriveBitPort operator[](পূর্ণাঙ্ক i) ধ্রুব
	{
		log_assert(i >= 0 && i < width);
		প্রত্যাগম DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort ধ্রুব &bit) ধ্রুব;
	bool try_append(DriveBitPort ধ্রুব &bit);
	bool try_append(DriveChunkPort ধ্রুব &chunk);

	// Whether this chunk is a whole port
	bool is_whole() ধ্রুব
	{
		প্রত্যাগম offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(ধ্রুব DriveChunkPort &other) ধ্রুব
	{
		প্রত্যাগম cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(ধ্রুব DriveChunkPort &other) ধ্রুব
	{
		যদি (cell != other.cell)
			প্রত্যাগম cell->name < other.cell->name;
		যদি (port != other.port)
			প্রত্যাগম port < other.port;
		যদি (width != other.width)
			প্রত্যাগম width < other.width;
		প্রত্যাগম offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;
};


সংরচনা DriveChunkMarker
{
	পূর্ণাঙ্ক marker;
	পূর্ণাঙ্ক offset;
	পূর্ণাঙ্ক width;

	DriveChunkMarker(পূর্ণাঙ্ক marker, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker ধ্রুব &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	পূর্ণাঙ্ক size() ধ্রুব { প্রত্যাগম width; }

	DriveBitMarker operator[](পূর্ণাঙ্ক i) ধ্রুব
	{
		log_assert(i >= 0 && i < width);
		প্রত্যাগম DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker ধ্রুব &bit) ধ্রুব;
	bool try_append(DriveBitMarker ধ্রুব &bit);
	bool try_append(DriveChunkMarker ধ্রুব &chunk);

	bool operator==(ধ্রুব DriveChunkMarker &other) ধ্রুব
	{
		প্রত্যাগম marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(ধ্রুব DriveChunkMarker &other) ধ্রুব
	{
		যদি (marker != other.marker)
			প্রত্যাগম marker < other.marker;
		যদি (width != other.width)
			প্রত্যাগম width < other.width;
		প্রত্যাগম offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;
};

সংরচনা DriveChunkMultiple
{
গুপ্ত:
	mutable pool<DriveChunk> multiple_;
	পূর্ণাঙ্ক width_;

সার্বজনিক:
	pool<DriveChunk> ধ্রুব &multiple() ধ্রুব { প্রত্যাগম multiple_; }

	DriveChunkMultiple(DriveBitMultiple ধ্রুব &bit);

	পূর্ণাঙ্ক size() ধ্রুব { প্রত্যাগম width_; }

	DriveBitMultiple operator[](পূর্ণাঙ্ক i) ধ্রুব;

	bool can_append(DriveBitMultiple ধ্রুব &bit) ধ্রুব;

	bool try_append(DriveBitMultiple ধ্রুব &bit);


	bool can_append(DriveChunkMultiple ধ্রুব &bit) ধ্রুব;

	bool try_append(DriveChunkMultiple ধ্রুব &bit);

	bool operator==(ধ্রুব DriveChunkMultiple &other) ধ্রুব
	{
		প্রত্যাগম width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(ধ্রুব DriveChunkMultiple &other) ধ্রুব
	{
		যদি (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		প্রত্যাগম false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;
};

সংরচনা DriveChunk
{
গুপ্ত:
	DriveType type_ = DriveType::NONE;
	সঙ্ঘ
	{
		পূর্ণাঙ্ক none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

সার্বজনিক:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk ধ্রুব &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit ধ্রুব &other) { *this = other; }

	DriveChunk(Const ধ্রুব &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ধ্রুব &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort ধ্রুব &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker ধ্রুব &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple ধ্রুব &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](পূর্ণাঙ্ক i) ধ্রুব
	{
		বিকল্প (type_)
		{
			পক্ষ DriveType::NONE:
				প্রত্যাগম DriveBit();
			পক্ষ DriveType::CONSTANT:
				প্রত্যাগম constant_[i];
			পক্ষ DriveType::WIRE:
				প্রত্যাগম wire_[i];
			পক্ষ DriveType::PORT:
				প্রত্যাগম port_[i];
			পক্ষ DriveType::MARKER:
				প্রত্যাগম marker_[i];
			পক্ষ DriveType::MULTIPLE:
				প্রত্যাগম multiple_[i];
		}
		log_abort();
	}

	শূন্য set_none(পূর্ণাঙ্ক width = 0)
	{
		বিকল্প (type_)
		{
			পক্ষ DriveType::NONE:
				none_ = width;
				ভঙ্গ;
			পক্ষ DriveType::CONSTANT:
				constant_.~Const();
				ভঙ্গ;
			পক্ষ DriveType::WIRE:
				wire_.~DriveChunkWire();
				ভঙ্গ;
			পক্ষ DriveType::PORT:
				port_.~DriveChunkPort();
				ভঙ্গ;
			পক্ষ DriveType::MARKER:
				marker_.~DriveChunkMarker();
				ভঙ্গ;
			পক্ষ DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				ভঙ্গ;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk ধ্রুব &other)
	{
		বিকল্প (other.type_)
		{
			পক্ষ DriveType::NONE:
				set_none(other.none_);
				ভঙ্গ;
			পক্ষ DriveType::CONSTANT:
				*this = other.constant_;
				ভঙ্গ;
			পক্ষ DriveType::WIRE:
				*this = other.wire_;
				ভঙ্গ;
			পক্ষ DriveType::PORT:
				*this = other.port_;
				ভঙ্গ;
			পক্ষ DriveType::MARKER:
				*this = other.marker_;
				ভঙ্গ;
			পক্ষ DriveType::MULTIPLE:
				*this = other.multiple_;
				ভঙ্গ;
		}
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		বিকল্প (other.type_)
		{
			পক্ষ DriveType::NONE:
				set_none(other.none_);
				ভঙ্গ;
			পক্ষ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ভঙ্গ;
			পক্ষ DriveType::WIRE:
				*this = std::move(other.wire_);
				ভঙ্গ;
			পক্ষ DriveType::PORT:
				*this = std::move(other.port_);
				ভঙ্গ;
			পক্ষ DriveType::MARKER:
				*this = std::move(other.marker_);
				ভঙ্গ;
			পক্ষ DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				ভঙ্গ;
		}
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(Const ধ্রুব &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveChunkWire ধ্রুব &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveChunkPort ধ্রুব &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveChunkMarker ধ্রুব &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveChunkMultiple ধ্রুব &multiple)
	{
		set_none(multiple.size());
		যদি (multiple.multiple().empty())
			প্রত্যাগম *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		যদি (multiple.multiple().empty())
			প্রত্যাগম *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		প্রত্যাগম *this;
	}

	DriveChunk &operator=(DriveBit ধ্রুব &other)
	{
		বিকল্প (other.type())
		{
			পক্ষ DriveType::NONE:
				set_none(1);
				ভঙ্গ;
			পক্ষ DriveType::CONSTANT:
				*this = Const(other.constant());
				ভঙ্গ;
			পক্ষ DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				ভঙ্গ;
			পক্ষ DriveType::PORT:
				*this = DriveChunkPort(other.port());
				ভঙ্গ;
			পক্ষ DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				ভঙ্গ;
			পক্ষ DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				ভঙ্গ;
		}
		প্রত্যাগম *this;
	}

	bool can_append(DriveBit ধ্রুব &bit) ধ্রুব;
	bool try_append(DriveBit ধ্রুব &bit);
	bool try_append(DriveChunk ধ্রুব &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;

	bool operator==(ধ্রুব DriveChunk &other) ধ্রুব
	{
		যদি (type_ != other.type_)
			প্রত্যাগম false;

		বিকল্প (type_)
		{
			পক্ষ DriveType::NONE:
				প্রত্যাগম true;
			পক্ষ DriveType::CONSTANT:
				প্রত্যাগম constant_ == other.constant_;
			পক্ষ DriveType::WIRE:
				প্রত্যাগম wire_ == other.wire_;
			পক্ষ DriveType::PORT:
				প্রত্যাগম port_ == other.port_;
			পক্ষ DriveType::MARKER:
				প্রত্যাগম marker_ == other.marker_;
			পক্ষ DriveType::MULTIPLE:
				প্রত্যাগম multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ধ্রুব DriveChunk &other) ধ্রুব
	{
		প্রত্যাগম !(*this == other);
	}

	bool operator<(ধ্রুব DriveChunk &other) ধ্রুব
	{
		যদি (type_ != other.type_)
			প্রত্যাগম type_ < other.type_;
		বিকল্প (type_)
		{
			পক্ষ DriveType::NONE:
				প্রত্যাগম false;
			পক্ষ DriveType::CONSTANT:
				প্রত্যাগম constant_ < other.constant_;
			পক্ষ DriveType::WIRE:
				প্রত্যাগম wire_ < other.wire_;
			পক্ষ DriveType::PORT:
				প্রত্যাগম port_ < other.port_;
			পক্ষ DriveType::MARKER:
				প্রত্যাগম marker_ < other.marker_;
			পক্ষ DriveType::MULTIPLE:
				প্রত্যাগম multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() ধ্রুব { প্রত্যাগম type_; }

	bool is_none() ধ্রুব { প্রত্যাগম type_ == DriveType::NONE; }
	bool is_constant() ধ্রুব { প্রত্যাগম type_ == DriveType::CONSTANT; }
	bool is_wire() ধ্রুব { প্রত্যাগম type_ == DriveType::WIRE; }
	bool is_port() ধ্রুব { প্রত্যাগম type_ == DriveType::PORT; }
	bool is_marker() ধ্রুব { প্রত্যাগম type_ == DriveType::MARKER; }
	bool is_multiple() ধ্রুব { প্রত্যাগম type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); প্রত্যাগম constant_; }
	Const ধ্রুব &constant() ধ্রুব { log_assert(is_constant()); প্রত্যাগম constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); প্রত্যাগম wire_; }
	DriveChunkWire ধ্রুব &wire() ধ্রুব { log_assert(is_wire()); প্রত্যাগম wire_; }
	DriveChunkPort &port() { log_assert(is_port()); প্রত্যাগম port_; }
	DriveChunkPort ধ্রুব &port() ধ্রুব { log_assert(is_port()); প্রত্যাগম port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); প্রত্যাগম marker_; }
	DriveChunkMarker ধ্রুব &marker() ধ্রুব { log_assert(is_marker()); প্রত্যাগম marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); প্রত্যাগম multiple_; }
	DriveChunkMultiple ধ্রুব &multiple() ধ্রুব { log_assert(is_multiple()); প্রত্যাগম multiple_; }


	পূর্ণাঙ্ক size() ধ্রুব
	{
		বিকল্প (type_)
		{
			পক্ষ DriveType::NONE:
				প্রত্যাগম none_;
			পক্ষ DriveType::CONSTANT:
				প্রত্যাগম constant_.size();
			পক্ষ DriveType::WIRE:
				প্রত্যাগম wire_.size();
			পক্ষ DriveType::PORT:
				প্রত্যাগম port_.size();
			পক্ষ DriveType::MARKER:
				প্রত্যাগম marker_.size();
			পক্ষ DriveType::MULTIPLE:
				প্রত্যাগম multiple_.size();
		}
		log_abort();
	}
};

অন্তঃস্থ DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple ধ্রুব &bit)
	: width_(1)
{
	প্রতি (auto ধ্রুব &bit : bit.multiple())
		multiple_.emplace(bit);
}

সংরচনা DriveSpec
{
গুপ্ত:
	পূর্ণাঙ্ক width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned পূর্ণাঙ্ক hash_ = 0;
সার্বজনিক:

	অন্তঃস্থ bool packed() ধ্রুব {
		প্রত্যাগম bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ধ্রুব &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ধ্রুব &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort ধ্রুব &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ধ্রুব &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple ধ্রুব &chunk) { *this = chunk; }

	DriveSpec(DriveBit ধ্রুব &bit) { *this = bit; }
	DriveSpec(DriveBitWire ধ্রুব &bit) { *this = bit; }
	DriveSpec(DriveBitPort ধ্রুব &bit) { *this = bit; }
	DriveSpec(DriveBitMarker ধ্রুব &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple ধ্রুব &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> ধ্রুব &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> ধ্রুব &bits)
	{
		প্রতি (auto ধ্রুব &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec ধ্রুব &sig)
	{
		// TODO: converting one chunk at a time would be faster
		প্রতি (auto ধ্রুব &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> ধ্রুব &chunks() ধ্রুব { pack(); প্রত্যাগম chunks_; }
	std::vector<DriveBit> ধ্রুব &bits() ধ্রুব { unpack(); প্রত্যাগম bits_; }

	পূর্ণাঙ্ক size() ধ্রুব { প্রত্যাগম width_; }

	শূন্য append(DriveBit ধ্রুব &bit);

	শূন্য append(DriveChunk ধ্রুব &chunk);

	শূন্য pack() ধ্রুব;

	শূন্য unpack() ধ্রুব;

	DriveBit &operator[](পূর্ণাঙ্ক index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		প্রত্যাগম bits_[index];
	}

	ধ্রুব DriveBit &operator[](পূর্ণাঙ্ক index) ধ্রুব
	{
		log_assert(index >= 0 && index < size());
		unpack();
		প্রত্যাগম bits_[index];
	}

	শূন্য clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk ধ্রুব &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		প্রত্যাগম *this;
	}

	DriveSpec &operator=(DriveChunkWire ধ্রুব &chunk) { প্রত্যাগম *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort ধ্রুব &chunk) { প্রত্যাগম *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker ধ্রুব &chunk) { প্রত্যাগম *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple ধ্রুব &chunk) { প্রত্যাগম *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit ধ্রুব &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		প্রত্যাগম *this;
	}

	DriveSpec &operator=(DriveBitWire ধ্রুব &bit) { প্রত্যাগম *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort ধ্রুব &bit) { প্রত্যাগম *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker ধ্রুব &bit) { প্রত্যাগম *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple ধ্রুব &bit) { প্রত্যাগম *this = DriveBit(bit); }

	শূন্য updhash() ধ্রুব {
		যদি (hash_ != 0)
			প্রত্যাগম;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;

	bool operator==(DriveSpec ধ্রুব &other) ধ্রুব {
		updhash();
		other.updhash();
		যদি (size() != other.size() || hash_ != other.hash_)
			প্রত্যাগম false;
		প্রত্যাগম chunks() == other.chunks();
	}

গুপ্ত:
	শূন্য compute_width();
};



সংরচনা DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

গুপ্ত:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	সংরচনা DriveBitId
	{
		পূর্ণাঙ্ক id = -1;

		DriveBitId() {};

		DriveBitId(পূর্ণাঙ্ক id) : id(id) { }

		bool operator==(ধ্রুব DriveBitId &other) ধ্রুব { প্রত্যাগম id == other.id; }
		bool operator!=(ধ্রুব DriveBitId &other) ধ্রুব { প্রত্যাগম id != other.id; }
		bool operator<(ধ্রুব DriveBitId &other) ধ্রুব { প্রত্যাগম id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	সংরচনা DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		শূন্য add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId বের_করো_edge(DriveBitId src);
		শূন্য clear(DriveBitId src);
		bool contains(DriveBitId src);
		পূর্ণাঙ্ক গুনতি(DriveBitId src);

		DriveBitId at(DriveBitId src, পূর্ণাঙ্ক index);
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
	পূর্ণাঙ্ক next_offset = 1 + (পূর্ণাঙ্ক)State::Sm;

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


	গণনা class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit ধ্রুব &bit);
	DriveBitId id_from_drive_bit(DriveBit ধ্রুব &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	শূন্য connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	শূন্য connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	শূন্য connect_undirected(DriveBitId a_id, DriveBitId b_id);

সার্বজনিক:

	শূন্য add(Module *module);

	// Add a single bit connection to the driver map.
	শূন্য add(DriveBit ধ্রুব &a, DriveBit ধ্রুব &b);

	template<typename T>
	স্থির constexpr bool is_sig_type() {
		প্রত্যাগম
			std::is_same<T, SigSpec>::মান ||
			std::is_same<T, SigChunk>::মান ||
			std::is_same<T, DriveSpec>::মান ||
			std::is_same<T, DriveChunk>::মান ||
			std::is_same<T, DriveChunkPort>::মান ||
			std::is_same<T, DriveChunkWire>::মান ||
			std::is_same<T, Const>::মান;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T ধ্রুব &a, U ধ্রুব &b)
	{
		log_assert(a.size() == b.size());
		প্রতি (পূর্ণাঙ্ক i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	শূন্য add(SigSpec ধ্রুব &a, SigSpec ধ্রুব &b);

গুপ্ত:
	শূন্য add_port(Cell *cell, IdString port, SigSpec ধ্রুব &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, পূর্ণাঙ্ক> orient_undirected_distance;

	শূন্য orient_undirected(DriveBitId id);

সার্বজনিক:
	DriveBit operator()(DriveBit ধ্রুব &bit);

	DriveSpec operator()(DriveSpec spec);

গুপ্ত:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		প্রত্যাগম wire->has_attribute(ID(keep));
	}
};

অন্তঃস্থ Hasher DriveBitWire::hash_into(Hasher h) ধ্রুব
{
	h.eat(wire->name);
	h.eat(offset);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveBitPort::hash_into(Hasher h) ধ্রুব
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveBitMarker::hash_into(Hasher h) ধ্রুব
{
	h.eat(marker);
	h.eat(offset);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveBitMultiple::hash_into(Hasher h) ধ্রুব
{
	h.eat(multiple_);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveBit::hash_into(Hasher h) ধ্রুব
{
	বিকল্প (type_) {
	পক্ষ DriveType::NONE:
		h.eat(0);
		ভঙ্গ;
	পক্ষ DriveType::CONSTANT:
		h.eat(constant_);
		ভঙ্গ;
	পক্ষ DriveType::WIRE:
		h.eat(wire_);
		ভঙ্গ;
	পক্ষ DriveType::PORT:
		h.eat(port_);
		ভঙ্গ;
	পক্ষ DriveType::MARKER:
		h.eat(marker_);
		ভঙ্গ;
	পক্ষ DriveType::MULTIPLE:
		h.eat(multiple_);
		ভঙ্গ;
	}
	h.eat(type_);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveChunkWire::hash_into(Hasher h) ধ্রুব
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveChunkPort::hash_into(Hasher h) ধ্রুব
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveChunkMarker::hash_into(Hasher h) ধ্রুব
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveChunkMultiple::hash_into(Hasher h) ধ্রুব
{
	h.eat(width_);
	h.eat(multiple_);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveChunk::hash_into(Hasher h) ধ্রুব
{
	বিকল্প (type_) {
	পক্ষ DriveType::NONE:
		h.eat(0);
		ভঙ্গ;
	পক্ষ DriveType::CONSTANT:
		h.eat(constant_);
		ভঙ্গ;
	পক্ষ DriveType::WIRE:
		h.eat(wire_);
		ভঙ্গ;
	পক্ষ DriveType::PORT:
		h.eat(port_);
		ভঙ্গ;
	পক্ষ DriveType::MARKER:
		h.eat(marker_);
		ভঙ্গ;
	পক্ষ DriveType::MULTIPLE:
		h.eat(multiple_);
		ভঙ্গ;
	}
	h.eat(type_);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriveSpec::hash_into(Hasher h) ধ্রুব
{
	updhash();
	h.eat(hash_);
	প্রত্যাগম h;
}

অন্তঃস্থ Hasher DriverMap::DriveBitId::hash_into(Hasher h) ধ্রুব
{
	h.eat(id);
	প্রত্যাগম h;
}

YOSYS_NAMESPACE_END

#endif
