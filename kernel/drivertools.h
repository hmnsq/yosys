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

#شامل <string>
#شامل <type_traits>

#شامل "kernel/rtlil.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

ساختار DriveBit;

ساختار DriveChunkWire;
ساختار DriveChunkPort;
ساختار DriveChunkMarker;
ساختار DriveChunk;

ساختار DriveSpec;

std::string log_signal(DriveChunkWire ثابت &chunk);
std::string log_signal(DriveChunkPort ثابت &chunk);
std::string log_signal(DriveChunkMarker ثابت &chunk);
std::string log_signal(DriveChunk ثابت &chunk);
std::string log_signal(DriveSpec ثابت &chunk);

شمارش class DriveType : unsigned نویسه
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

ساختار DriveBitWire
{
	Wire *wire;
	صحیح offset;

	DriveBitWire(Wire *wire, صحیح offset) : wire(wire), offset(offset) {}

	bool operator==(ثابت DriveBitWire &other) ثابت
	{
		بازگشت wire == other.wire && offset == other.offset;
	}

	bool operator<(ثابت DriveBitWire &other) ثابت
	{
		اگر (wire != other.wire)
			بازگشت wire->name < other.wire->name;
		بازگشت offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;


	operator SigBit() ثابت
	{
		بازگشت SigBit(wire, offset);
	}
};

ساختار DriveBitPort
{
	Cell *cell;
	IdString port;
	صحیح offset;

	DriveBitPort(Cell *cell, IdString port, صحیح offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(ثابت DriveBitPort &other) ثابت
	{
		بازگشت cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(ثابت DriveBitPort &other) ثابت
	{
		اگر (cell != other.cell)
			بازگشت cell->name < other.cell->name;
		اگر (port != other.port)
			بازگشت port < other.port;
		بازگشت offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};


ساختار DriveBitMarker
{
	صحیح marker;
	صحیح offset;

	DriveBitMarker(صحیح marker, صحیح offset) : marker(marker), offset(offset) {}

	bool operator==(ثابت DriveBitMarker &other) ثابت
	{
		بازگشت marker == other.marker && offset == other.offset;
	}

	bool operator<(ثابت DriveBitMarker &other) ثابت
	{
		اگر (marker != other.marker)
			بازگشت marker < other.marker;
		بازگشت offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};

ساختار DriveBitMultiple
{
خصوصی:
	pool<DriveBit> multiple_;

عمومی:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit ثابت &single);

	pool<DriveBit> ثابت &multiple() ثابت { بازگشت multiple_; }

	تهی merge(DriveBitMultiple ثابت &other)
	{
		برای (DriveBit ثابت &single : other.multiple_)
			merge(single);
	}

	تهی merge(DriveBitMultiple &&other)
	{
		برای (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	تهی merge(DriveBit ثابت &single);
	تهی merge(DriveBit &&single);

	bool operator==(ثابت DriveBitMultiple &other) ثابت
	{
		بازگشت multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

ساختار DriveBit
{
خصوصی:
	DriveType type_ = DriveType::NONE;
	اجتماع
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
عمومی:
	DriveBit()  {}

	DriveBit(SigBit ثابت &bit);

	DriveBit(DriveBit ثابت &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire ثابت &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort ثابت &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker ثابت &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple ثابت &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	تهی set_none()
	{
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				بشکن;
			حالت DriveType::CONSTANT:
				بشکن;
			حالت DriveType::WIRE:
				wire_.~DriveBitWire();
				بشکن;
			حالت DriveType::PORT:
				port_.~DriveBitPort();
				بشکن;
			حالت DriveType::MARKER:
				marker_.~DriveBitMarker();
				بشکن;
			حالت DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				بشکن;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit ثابت &other)
	{
		انتخاب (other.type_)
		{
			حالت DriveType::NONE:
				set_none();
				بشکن;
			حالت DriveType::CONSTANT:
				*this = other.constant_;
				بشکن;
			حالت DriveType::WIRE:
				*this = other.wire_;
				بشکن;
			حالت DriveType::PORT:
				*this = other.port_;
				بشکن;
			حالت DriveType::MARKER:
				*this = other.marker_;
				بشکن;
			حالت DriveType::MULTIPLE:
				*this = other.multiple_;
				بشکن;
		}
		بازگشت *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		انتخاب (other.type_)
		{
			حالت DriveType::NONE:
				set_none();
				بشکن;
			حالت DriveType::CONSTANT:
				*this = std::move(other.constant_);
				بشکن;
			حالت DriveType::WIRE:
				*this = std::move(other.wire_);
				بشکن;
			حالت DriveType::PORT:
				*this = std::move(other.port_);
				بشکن;
			حالت DriveType::MARKER:
				*this = std::move(other.marker_);
				بشکن;
			حالت DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				بشکن;
		}
		بازگشت *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		بازگشت *this;
	}

	DriveBit &operator=(DriveBitWire ثابت &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		بازگشت *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		بازگشت *this;
	}

	DriveBit &operator=(DriveBitPort ثابت &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		بازگشت *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		بازگشت *this;
	}

	DriveBit &operator=(DriveBitMarker ثابت &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		بازگشت *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		بازگشت *this;
	}

	DriveBit &operator=(DriveBitMultiple ثابت &multiple)
	{
		set_none();
		اگر (multiple.multiple().empty())
			بازگشت *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		بازگشت *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		اگر (multiple.multiple().empty())
			بازگشت *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		بازگشت *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool operator==(ثابت DriveBit &other) ثابت
	{
		اگر (type_ != other.type_)
			بازگشت false;

		انتخاب (type_)
		{
			حالت DriveType::NONE:
				بازگشت true;
			حالت DriveType::CONSTANT:
				بازگشت constant_ == other.constant_;
			حالت DriveType::WIRE:
				بازگشت wire_ == other.wire_;
			حالت DriveType::PORT:
				بازگشت port_ == other.port_;
			حالت DriveType::MARKER:
				بازگشت marker_ == other.marker_;
			حالت DriveType::MULTIPLE:
				بازگشت multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ثابت DriveBit &other) ثابت
	{
		بازگشت !(*this == other);
	}

	bool operator<(ثابت DriveBit &other) ثابت
	{
		اگر (type_ != other.type_)
			بازگشت type_ < other.type_;
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				بازگشت false;
			حالت DriveType::CONSTANT:
				بازگشت constant_ < other.constant_;
			حالت DriveType::WIRE:
				بازگشت wire_ < other.wire_;
			حالت DriveType::PORT:
				بازگشت port_ < other.port_;
			حالت DriveType::MARKER:
				بازگشت marker_ < other.marker_;
			حالت DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ثابت { بازگشت type_; }

	bool is_none() ثابت { بازگشت type_ == DriveType::NONE; }
	bool is_constant() ثابت { بازگشت type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { بازگشت type_ == DriveType::WIRE; }
	bool is_port() ثابت { بازگشت type_ == DriveType::PORT; }
	bool is_marker() ثابت { بازگشت type_ == DriveType::MARKER; }
	bool is_multiple() ثابت { بازگشت type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); بازگشت constant_; }
	State ثابت &constant() ثابت { log_assert(is_constant()); بازگشت constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); بازگشت wire_; }
	DriveBitWire ثابت &wire() ثابت { log_assert(is_wire()); بازگشت wire_; }
	DriveBitPort &port() { log_assert(is_port()); بازگشت port_; }
	DriveBitPort ثابت &port() ثابت { log_assert(is_port()); بازگشت port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); بازگشت marker_; }
	DriveBitMarker ثابت &marker() ثابت { log_assert(is_marker()); بازگشت marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); بازگشت multiple_; }
	DriveBitMultiple ثابت &multiple() ثابت { log_assert(is_multiple()); بازگشت multiple_; }

	تهی merge(DriveBit ثابت &other);

};

درونخطی DriveBitMultiple::DriveBitMultiple() {}
درونخطی DriveBitMultiple::DriveBitMultiple(DriveBit ثابت &single)
{
	multiple_.emplace(single);
}

ساختار DriveChunkWire
{
	Wire *wire;
	صحیح offset;
	صحیح width;

	DriveChunkWire(Wire *wire, صحیح offset, صحیح width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire ثابت &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	صحیح size() ثابت { بازگشت width; }

	DriveBitWire operator[](صحیح i) ثابت
	{
		log_assert(i >= 0 && i < width);
		بازگشت DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire ثابت &bit) ثابت;
	bool try_append(DriveBitWire ثابت &bit);
	bool try_append(DriveChunkWire ثابت &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() ثابت { بازگشت offset == 0 && width == wire->width; }

	bool operator==(ثابت DriveChunkWire &other) ثابت
	{
		بازگشت wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(ثابت DriveChunkWire &other) ثابت
	{
		اگر (wire != other.wire)
			بازگشت wire->name < other.wire->name;
		اگر (width != other.width)
			بازگشت width < other.width;
		بازگشت offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	explicit operator SigChunk() ثابت
	{
		بازگشت SigChunk(wire, offset, width);
	}
};

ساختار DriveChunkPort
{
	Cell *cell;
	IdString port;
	صحیح offset;
	صحیح width;

	DriveChunkPort(Cell *cell, IdString port, صحیح offset, صحیح width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> ثابت &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort ثابت &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	صحیح size() ثابت { بازگشت width; }

	DriveBitPort operator[](صحیح i) ثابت
	{
		log_assert(i >= 0 && i < width);
		بازگشت DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort ثابت &bit) ثابت;
	bool try_append(DriveBitPort ثابت &bit);
	bool try_append(DriveChunkPort ثابت &chunk);

	// Whether this chunk is a whole port
	bool is_whole() ثابت
	{
		بازگشت offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(ثابت DriveChunkPort &other) ثابت
	{
		بازگشت cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(ثابت DriveChunkPort &other) ثابت
	{
		اگر (cell != other.cell)
			بازگشت cell->name < other.cell->name;
		اگر (port != other.port)
			بازگشت port < other.port;
		اگر (width != other.width)
			بازگشت width < other.width;
		بازگشت offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};


ساختار DriveChunkMarker
{
	صحیح marker;
	صحیح offset;
	صحیح width;

	DriveChunkMarker(صحیح marker, صحیح offset, صحیح width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker ثابت &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	صحیح size() ثابت { بازگشت width; }

	DriveBitMarker operator[](صحیح i) ثابت
	{
		log_assert(i >= 0 && i < width);
		بازگشت DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker ثابت &bit) ثابت;
	bool try_append(DriveBitMarker ثابت &bit);
	bool try_append(DriveChunkMarker ثابت &chunk);

	bool operator==(ثابت DriveChunkMarker &other) ثابت
	{
		بازگشت marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(ثابت DriveChunkMarker &other) ثابت
	{
		اگر (marker != other.marker)
			بازگشت marker < other.marker;
		اگر (width != other.width)
			بازگشت width < other.width;
		بازگشت offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

ساختار DriveChunkMultiple
{
خصوصی:
	mutable pool<DriveChunk> multiple_;
	صحیح width_;

عمومی:
	pool<DriveChunk> ثابت &multiple() ثابت { بازگشت multiple_; }

	DriveChunkMultiple(DriveBitMultiple ثابت &bit);

	صحیح size() ثابت { بازگشت width_; }

	DriveBitMultiple operator[](صحیح i) ثابت;

	bool can_append(DriveBitMultiple ثابت &bit) ثابت;

	bool try_append(DriveBitMultiple ثابت &bit);


	bool can_append(DriveChunkMultiple ثابت &bit) ثابت;

	bool try_append(DriveChunkMultiple ثابت &bit);

	bool operator==(ثابت DriveChunkMultiple &other) ثابت
	{
		بازگشت width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(ثابت DriveChunkMultiple &other) ثابت
	{
		اگر (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		بازگشت false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

ساختار DriveChunk
{
خصوصی:
	DriveType type_ = DriveType::NONE;
	اجتماع
	{
		صحیح none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

عمومی:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk ثابت &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit ثابت &other) { *this = other; }

	DriveChunk(Const ثابت &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ثابت &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort ثابت &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker ثابت &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple ثابت &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](صحیح i) ثابت
	{
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				بازگشت DriveBit();
			حالت DriveType::CONSTANT:
				بازگشت constant_[i];
			حالت DriveType::WIRE:
				بازگشت wire_[i];
			حالت DriveType::PORT:
				بازگشت port_[i];
			حالت DriveType::MARKER:
				بازگشت marker_[i];
			حالت DriveType::MULTIPLE:
				بازگشت multiple_[i];
		}
		log_abort();
	}

	تهی set_none(صحیح width = 0)
	{
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				none_ = width;
				بشکن;
			حالت DriveType::CONSTANT:
				constant_.~Const();
				بشکن;
			حالت DriveType::WIRE:
				wire_.~DriveChunkWire();
				بشکن;
			حالت DriveType::PORT:
				port_.~DriveChunkPort();
				بشکن;
			حالت DriveType::MARKER:
				marker_.~DriveChunkMarker();
				بشکن;
			حالت DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				بشکن;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk ثابت &other)
	{
		انتخاب (other.type_)
		{
			حالت DriveType::NONE:
				set_none(other.none_);
				بشکن;
			حالت DriveType::CONSTANT:
				*this = other.constant_;
				بشکن;
			حالت DriveType::WIRE:
				*this = other.wire_;
				بشکن;
			حالت DriveType::PORT:
				*this = other.port_;
				بشکن;
			حالت DriveType::MARKER:
				*this = other.marker_;
				بشکن;
			حالت DriveType::MULTIPLE:
				*this = other.multiple_;
				بشکن;
		}
		بازگشت *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		انتخاب (other.type_)
		{
			حالت DriveType::NONE:
				set_none(other.none_);
				بشکن;
			حالت DriveType::CONSTANT:
				*this = std::move(other.constant_);
				بشکن;
			حالت DriveType::WIRE:
				*this = std::move(other.wire_);
				بشکن;
			حالت DriveType::PORT:
				*this = std::move(other.port_);
				بشکن;
			حالت DriveType::MARKER:
				*this = std::move(other.marker_);
				بشکن;
			حالت DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				بشکن;
		}
		بازگشت *this;
	}

	DriveChunk &operator=(Const ثابت &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		بازگشت *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		بازگشت *this;
	}

	DriveChunk &operator=(DriveChunkWire ثابت &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		بازگشت *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		بازگشت *this;
	}

	DriveChunk &operator=(DriveChunkPort ثابت &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		بازگشت *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		بازگشت *this;
	}

	DriveChunk &operator=(DriveChunkMarker ثابت &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		بازگشت *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		بازگشت *this;
	}

	DriveChunk &operator=(DriveChunkMultiple ثابت &multiple)
	{
		set_none(multiple.size());
		اگر (multiple.multiple().empty())
			بازگشت *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		بازگشت *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		اگر (multiple.multiple().empty())
			بازگشت *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		بازگشت *this;
	}

	DriveChunk &operator=(DriveBit ثابت &other)
	{
		انتخاب (other.type())
		{
			حالت DriveType::NONE:
				set_none(1);
				بشکن;
			حالت DriveType::CONSTANT:
				*this = Const(other.constant());
				بشکن;
			حالت DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				بشکن;
			حالت DriveType::PORT:
				*this = DriveChunkPort(other.port());
				بشکن;
			حالت DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				بشکن;
			حالت DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				بشکن;
		}
		بازگشت *this;
	}

	bool can_append(DriveBit ثابت &bit) ثابت;
	bool try_append(DriveBit ثابت &bit);
	bool try_append(DriveChunk ثابت &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool operator==(ثابت DriveChunk &other) ثابت
	{
		اگر (type_ != other.type_)
			بازگشت false;

		انتخاب (type_)
		{
			حالت DriveType::NONE:
				بازگشت true;
			حالت DriveType::CONSTANT:
				بازگشت constant_ == other.constant_;
			حالت DriveType::WIRE:
				بازگشت wire_ == other.wire_;
			حالت DriveType::PORT:
				بازگشت port_ == other.port_;
			حالت DriveType::MARKER:
				بازگشت marker_ == other.marker_;
			حالت DriveType::MULTIPLE:
				بازگشت multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ثابت DriveChunk &other) ثابت
	{
		بازگشت !(*this == other);
	}

	bool operator<(ثابت DriveChunk &other) ثابت
	{
		اگر (type_ != other.type_)
			بازگشت type_ < other.type_;
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				بازگشت false;
			حالت DriveType::CONSTANT:
				بازگشت constant_ < other.constant_;
			حالت DriveType::WIRE:
				بازگشت wire_ < other.wire_;
			حالت DriveType::PORT:
				بازگشت port_ < other.port_;
			حالت DriveType::MARKER:
				بازگشت marker_ < other.marker_;
			حالت DriveType::MULTIPLE:
				بازگشت multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() ثابت { بازگشت type_; }

	bool is_none() ثابت { بازگشت type_ == DriveType::NONE; }
	bool is_constant() ثابت { بازگشت type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { بازگشت type_ == DriveType::WIRE; }
	bool is_port() ثابت { بازگشت type_ == DriveType::PORT; }
	bool is_marker() ثابت { بازگشت type_ == DriveType::MARKER; }
	bool is_multiple() ثابت { بازگشت type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); بازگشت constant_; }
	Const ثابت &constant() ثابت { log_assert(is_constant()); بازگشت constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); بازگشت wire_; }
	DriveChunkWire ثابت &wire() ثابت { log_assert(is_wire()); بازگشت wire_; }
	DriveChunkPort &port() { log_assert(is_port()); بازگشت port_; }
	DriveChunkPort ثابت &port() ثابت { log_assert(is_port()); بازگشت port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); بازگشت marker_; }
	DriveChunkMarker ثابت &marker() ثابت { log_assert(is_marker()); بازگشت marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); بازگشت multiple_; }
	DriveChunkMultiple ثابت &multiple() ثابت { log_assert(is_multiple()); بازگشت multiple_; }


	صحیح size() ثابت
	{
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				بازگشت none_;
			حالت DriveType::CONSTANT:
				بازگشت constant_.size();
			حالت DriveType::WIRE:
				بازگشت wire_.size();
			حالت DriveType::PORT:
				بازگشت port_.size();
			حالت DriveType::MARKER:
				بازگشت marker_.size();
			حالت DriveType::MULTIPLE:
				بازگشت multiple_.size();
		}
		log_abort();
	}
};

درونخطی DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple ثابت &bit)
	: width_(1)
{
	برای (auto ثابت &bit : bit.multiple())
		multiple_.emplace(bit);
}

ساختار DriveSpec
{
خصوصی:
	صحیح width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned صحیح hash_ = 0;
عمومی:

	درونخطی bool packed() ثابت {
		بازگشت bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple ثابت &chunk) { *this = chunk; }

	DriveSpec(DriveBit ثابت &bit) { *this = bit; }
	DriveSpec(DriveBitWire ثابت &bit) { *this = bit; }
	DriveSpec(DriveBitPort ثابت &bit) { *this = bit; }
	DriveSpec(DriveBitMarker ثابت &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple ثابت &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> ثابت &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> ثابت &bits)
	{
		برای (auto ثابت &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec ثابت &sig)
	{
		// TODO: converting one chunk at a time would be faster
		برای (auto ثابت &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> ثابت &chunks() ثابت { pack(); بازگشت chunks_; }
	std::vector<DriveBit> ثابت &bits() ثابت { unpack(); بازگشت bits_; }

	صحیح size() ثابت { بازگشت width_; }

	تهی append(DriveBit ثابت &bit);

	تهی append(DriveChunk ثابت &chunk);

	تهی pack() ثابت;

	تهی unpack() ثابت;

	DriveBit &operator[](صحیح index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		بازگشت bits_[index];
	}

	ثابت DriveBit &operator[](صحیح index) ثابت
	{
		log_assert(index >= 0 && index < size());
		unpack();
		بازگشت bits_[index];
	}

	تهی clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk ثابت &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		بازگشت *this;
	}

	DriveSpec &operator=(DriveChunkWire ثابت &chunk) { بازگشت *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort ثابت &chunk) { بازگشت *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker ثابت &chunk) { بازگشت *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple ثابت &chunk) { بازگشت *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit ثابت &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		بازگشت *this;
	}

	DriveSpec &operator=(DriveBitWire ثابت &bit) { بازگشت *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort ثابت &bit) { بازگشت *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker ثابت &bit) { بازگشت *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple ثابت &bit) { بازگشت *this = DriveBit(bit); }

	تهی updhash() ثابت {
		اگر (hash_ != 0)
			بازگشت;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool operator==(DriveSpec ثابت &other) ثابت {
		updhash();
		other.updhash();
		اگر (size() != other.size() || hash_ != other.hash_)
			بازگشت false;
		بازگشت chunks() == other.chunks();
	}

خصوصی:
	تهی compute_width();
};



ساختار DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

خصوصی:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	ساختار DriveBitId
	{
		صحیح id = -1;

		DriveBitId() {};

		DriveBitId(صحیح id) : id(id) { }

		bool operator==(ثابت DriveBitId &other) ثابت { بازگشت id == other.id; }
		bool operator!=(ثابت DriveBitId &other) ثابت { بازگشت id != other.id; }
		bool operator<(ثابت DriveBitId &other) ثابت { بازگشت id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	ساختار DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		تهی add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId برداشت_edge(DriveBitId src);
		تهی clear(DriveBitId src);
		bool contains(DriveBitId src);
		صحیح شمار(DriveBitId src);

		DriveBitId at(DriveBitId src, صحیح index);
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
	صحیح next_offset = 1 + (صحیح)State::Sm;

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


	شمارش class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit ثابت &bit);
	DriveBitId id_from_drive_bit(DriveBit ثابت &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	تهی connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	تهی connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	تهی connect_undirected(DriveBitId a_id, DriveBitId b_id);

عمومی:

	تهی add(Module *module);

	// Add a single bit connection to the driver map.
	تهی add(DriveBit ثابت &a, DriveBit ثابت &b);

	template<typename T>
	ایستا constexpr bool is_sig_type() {
		بازگشت
			std::is_same<T, SigSpec>::مقدار ||
			std::is_same<T, SigChunk>::مقدار ||
			std::is_same<T, DriveSpec>::مقدار ||
			std::is_same<T, DriveChunk>::مقدار ||
			std::is_same<T, DriveChunkPort>::مقدار ||
			std::is_same<T, DriveChunkWire>::مقدار ||
			std::is_same<T, Const>::مقدار;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T ثابت &a, U ثابت &b)
	{
		log_assert(a.size() == b.size());
		برای (صحیح i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	تهی add(SigSpec ثابت &a, SigSpec ثابت &b);

خصوصی:
	تهی add_port(Cell *cell, IdString port, SigSpec ثابت &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, صحیح> orient_undirected_distance;

	تهی orient_undirected(DriveBitId id);

عمومی:
	DriveBit operator()(DriveBit ثابت &bit);

	DriveSpec operator()(DriveSpec spec);

خصوصی:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		بازگشت wire->has_attribute(ID(keep));
	}
};

درونخطی Hasher DriveBitWire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(offset);
	بازگشت h;
}

درونخطی Hasher DriveBitPort::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	بازگشت h;
}

درونخطی Hasher DriveBitMarker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(offset);
	بازگشت h;
}

درونخطی Hasher DriveBitMultiple::hash_into(Hasher h) ثابت
{
	h.eat(multiple_);
	بازگشت h;
}

درونخطی Hasher DriveBit::hash_into(Hasher h) ثابت
{
	انتخاب (type_) {
	حالت DriveType::NONE:
		h.eat(0);
		بشکن;
	حالت DriveType::CONSTANT:
		h.eat(constant_);
		بشکن;
	حالت DriveType::WIRE:
		h.eat(wire_);
		بشکن;
	حالت DriveType::PORT:
		h.eat(port_);
		بشکن;
	حالت DriveType::MARKER:
		h.eat(marker_);
		بشکن;
	حالت DriveType::MULTIPLE:
		h.eat(multiple_);
		بشکن;
	}
	h.eat(type_);
	بازگشت h;
}

درونخطی Hasher DriveChunkWire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	بازگشت h;
}

درونخطی Hasher DriveChunkPort::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	بازگشت h;
}

درونخطی Hasher DriveChunkMarker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	بازگشت h;
}

درونخطی Hasher DriveChunkMultiple::hash_into(Hasher h) ثابت
{
	h.eat(width_);
	h.eat(multiple_);
	بازگشت h;
}

درونخطی Hasher DriveChunk::hash_into(Hasher h) ثابت
{
	انتخاب (type_) {
	حالت DriveType::NONE:
		h.eat(0);
		بشکن;
	حالت DriveType::CONSTANT:
		h.eat(constant_);
		بشکن;
	حالت DriveType::WIRE:
		h.eat(wire_);
		بشکن;
	حالت DriveType::PORT:
		h.eat(port_);
		بشکن;
	حالت DriveType::MARKER:
		h.eat(marker_);
		بشکن;
	حالت DriveType::MULTIPLE:
		h.eat(multiple_);
		بشکن;
	}
	h.eat(type_);
	بازگشت h;
}

درونخطی Hasher DriveSpec::hash_into(Hasher h) ثابت
{
	updhash();
	h.eat(hash_);
	بازگشت h;
}

درونخطی Hasher DriverMap::DriveBitId::hash_into(Hasher h) ثابت
{
	h.eat(id);
	بازگشت h;
}

YOSYS_NAMESPACE_END

#endif
