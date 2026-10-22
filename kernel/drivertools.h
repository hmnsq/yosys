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

কাঠামো DriveBit;

কাঠামো DriveChunkWire;
কাঠামো DriveChunkPort;
কাঠামো DriveChunkMarker;
কাঠামো DriveChunk;

কাঠামো DriveSpec;

std::string log_signal(DriveChunkWire ধ্রুবক &chunk);
std::string log_signal(DriveChunkPort ধ্রুবক &chunk);
std::string log_signal(DriveChunkMarker ধ্রুবক &chunk);
std::string log_signal(DriveChunk ধ্রুবক &chunk);
std::string log_signal(DriveSpec ধ্রুবক &chunk);

গণনা class DriveType : unsigned অক্ষর
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

কাঠামো DriveBitWire
{
	Wire *wire;
	পূর্ণসংখ্যা offset;

	DriveBitWire(Wire *wire, পূর্ণসংখ্যা offset) : wire(wire), offset(offset) {}

	bool operator==(ধ্রুবক DriveBitWire &other) ধ্রুবক
	{
		ফেরত wire == other.wire && offset == other.offset;
	}

	bool operator<(ধ্রুবক DriveBitWire &other) ধ্রুবক
	{
		যদি (wire != other.wire)
			ফেরত wire->name < other.wire->name;
		ফেরত offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;


	operator SigBit() ধ্রুবক
	{
		ফেরত SigBit(wire, offset);
	}
};

কাঠামো DriveBitPort
{
	Cell *cell;
	IdString port;
	পূর্ণসংখ্যা offset;

	DriveBitPort(Cell *cell, IdString port, পূর্ণসংখ্যা offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(ধ্রুবক DriveBitPort &other) ধ্রুবক
	{
		ফেরত cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(ধ্রুবক DriveBitPort &other) ধ্রুবক
	{
		যদি (cell != other.cell)
			ফেরত cell->name < other.cell->name;
		যদি (port != other.port)
			ফেরত port < other.port;
		ফেরত offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;

};


কাঠামো DriveBitMarker
{
	পূর্ণসংখ্যা marker;
	পূর্ণসংখ্যা offset;

	DriveBitMarker(পূর্ণসংখ্যা marker, পূর্ণসংখ্যা offset) : marker(marker), offset(offset) {}

	bool operator==(ধ্রুবক DriveBitMarker &other) ধ্রুবক
	{
		ফেরত marker == other.marker && offset == other.offset;
	}

	bool operator<(ধ্রুবক DriveBitMarker &other) ধ্রুবক
	{
		যদি (marker != other.marker)
			ফেরত marker < other.marker;
		ফেরত offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;

};

কাঠামো DriveBitMultiple
{
ব্যক্তিগত:
	pool<DriveBit> multiple_;

সর্বজনীন:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit ধ্রুবক &single);

	pool<DriveBit> ধ্রুবক &multiple() ধ্রুবক { ফেরত multiple_; }

	শূন্য merge(DriveBitMultiple ধ্রুবক &other)
	{
		জন্য (DriveBit ধ্রুবক &single : other.multiple_)
			merge(single);
	}

	শূন্য merge(DriveBitMultiple &&other)
	{
		জন্য (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	শূন্য merge(DriveBit ধ্রুবক &single);
	শূন্য merge(DriveBit &&single);

	bool operator==(ধ্রুবক DriveBitMultiple &other) ধ্রুবক
	{
		ফেরত multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;
};

কাঠামো DriveBit
{
ব্যক্তিগত:
	DriveType type_ = DriveType::NONE;
	সংঘ
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
সর্বজনীন:
	DriveBit()  {}

	DriveBit(SigBit ধ্রুবক &bit);

	DriveBit(DriveBit ধ্রুবক &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire ধ্রুবক &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort ধ্রুবক &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker ধ্রুবক &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple ধ্রুবক &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	শূন্য set_none()
	{
		নির্বাচন (type_)
		{
			ক্ষেত্র DriveType::NONE:
				বিরতি;
			ক্ষেত্র DriveType::CONSTANT:
				বিরতি;
			ক্ষেত্র DriveType::WIRE:
				wire_.~DriveBitWire();
				বিরতি;
			ক্ষেত্র DriveType::PORT:
				port_.~DriveBitPort();
				বিরতি;
			ক্ষেত্র DriveType::MARKER:
				marker_.~DriveBitMarker();
				বিরতি;
			ক্ষেত্র DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				বিরতি;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit ধ্রুবক &other)
	{
		নির্বাচন (other.type_)
		{
			ক্ষেত্র DriveType::NONE:
				set_none();
				বিরতি;
			ক্ষেত্র DriveType::CONSTANT:
				*this = other.constant_;
				বিরতি;
			ক্ষেত্র DriveType::WIRE:
				*this = other.wire_;
				বিরতি;
			ক্ষেত্র DriveType::PORT:
				*this = other.port_;
				বিরতি;
			ক্ষেত্র DriveType::MARKER:
				*this = other.marker_;
				বিরতি;
			ক্ষেত্র DriveType::MULTIPLE:
				*this = other.multiple_;
				বিরতি;
		}
		ফেরত *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		নির্বাচন (other.type_)
		{
			ক্ষেত্র DriveType::NONE:
				set_none();
				বিরতি;
			ক্ষেত্র DriveType::CONSTANT:
				*this = std::move(other.constant_);
				বিরতি;
			ক্ষেত্র DriveType::WIRE:
				*this = std::move(other.wire_);
				বিরতি;
			ক্ষেত্র DriveType::PORT:
				*this = std::move(other.port_);
				বিরতি;
			ক্ষেত্র DriveType::MARKER:
				*this = std::move(other.marker_);
				বিরতি;
			ক্ষেত্র DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				বিরতি;
		}
		ফেরত *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		ফেরত *this;
	}

	DriveBit &operator=(DriveBitWire ধ্রুবক &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ফেরত *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ফেরত *this;
	}

	DriveBit &operator=(DriveBitPort ধ্রুবক &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ফেরত *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ফেরত *this;
	}

	DriveBit &operator=(DriveBitMarker ধ্রুবক &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ফেরত *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ফেরত *this;
	}

	DriveBit &operator=(DriveBitMultiple ধ্রুবক &multiple)
	{
		set_none();
		যদি (multiple.multiple().empty())
			ফেরত *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ফেরত *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		যদি (multiple.multiple().empty())
			ফেরত *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ফেরত *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;

	bool operator==(ধ্রুবক DriveBit &other) ধ্রুবক
	{
		যদি (type_ != other.type_)
			ফেরত false;

		নির্বাচন (type_)
		{
			ক্ষেত্র DriveType::NONE:
				ফেরত true;
			ক্ষেত্র DriveType::CONSTANT:
				ফেরত constant_ == other.constant_;
			ক্ষেত্র DriveType::WIRE:
				ফেরত wire_ == other.wire_;
			ক্ষেত্র DriveType::PORT:
				ফেরত port_ == other.port_;
			ক্ষেত্র DriveType::MARKER:
				ফেরত marker_ == other.marker_;
			ক্ষেত্র DriveType::MULTIPLE:
				ফেরত multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ধ্রুবক DriveBit &other) ধ্রুবক
	{
		ফেরত !(*this == other);
	}

	bool operator<(ধ্রুবক DriveBit &other) ধ্রুবক
	{
		যদি (type_ != other.type_)
			ফেরত type_ < other.type_;
		নির্বাচন (type_)
		{
			ক্ষেত্র DriveType::NONE:
				ফেরত false;
			ক্ষেত্র DriveType::CONSTANT:
				ফেরত constant_ < other.constant_;
			ক্ষেত্র DriveType::WIRE:
				ফেরত wire_ < other.wire_;
			ক্ষেত্র DriveType::PORT:
				ফেরত port_ < other.port_;
			ক্ষেত্র DriveType::MARKER:
				ফেরত marker_ < other.marker_;
			ক্ষেত্র DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ধ্রুবক { ফেরত type_; }

	bool is_none() ধ্রুবক { ফেরত type_ == DriveType::NONE; }
	bool is_constant() ধ্রুবক { ফেরত type_ == DriveType::CONSTANT; }
	bool is_wire() ধ্রুবক { ফেরত type_ == DriveType::WIRE; }
	bool is_port() ধ্রুবক { ফেরত type_ == DriveType::PORT; }
	bool is_marker() ধ্রুবক { ফেরত type_ == DriveType::MARKER; }
	bool is_multiple() ধ্রুবক { ফেরত type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); ফেরত constant_; }
	State ধ্রুবক &constant() ধ্রুবক { log_assert(is_constant()); ফেরত constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); ফেরত wire_; }
	DriveBitWire ধ্রুবক &wire() ধ্রুবক { log_assert(is_wire()); ফেরত wire_; }
	DriveBitPort &port() { log_assert(is_port()); ফেরত port_; }
	DriveBitPort ধ্রুবক &port() ধ্রুবক { log_assert(is_port()); ফেরত port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); ফেরত marker_; }
	DriveBitMarker ধ্রুবক &marker() ধ্রুবক { log_assert(is_marker()); ফেরত marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); ফেরত multiple_; }
	DriveBitMultiple ধ্রুবক &multiple() ধ্রুবক { log_assert(is_multiple()); ফেরত multiple_; }

	শূন্য merge(DriveBit ধ্রুবক &other);

};

অন্তঃরেখ DriveBitMultiple::DriveBitMultiple() {}
অন্তঃরেখ DriveBitMultiple::DriveBitMultiple(DriveBit ধ্রুবক &single)
{
	multiple_.emplace(single);
}

কাঠামো DriveChunkWire
{
	Wire *wire;
	পূর্ণসংখ্যা offset;
	পূর্ণসংখ্যা width;

	DriveChunkWire(Wire *wire, পূর্ণসংখ্যা offset, পূর্ণসংখ্যা width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire ধ্রুবক &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত width; }

	DriveBitWire operator[](পূর্ণসংখ্যা i) ধ্রুবক
	{
		log_assert(i >= 0 && i < width);
		ফেরত DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire ধ্রুবক &bit) ধ্রুবক;
	bool try_append(DriveBitWire ধ্রুবক &bit);
	bool try_append(DriveChunkWire ধ্রুবক &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() ধ্রুবক { ফেরত offset == 0 && width == wire->width; }

	bool operator==(ধ্রুবক DriveChunkWire &other) ধ্রুবক
	{
		ফেরত wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(ধ্রুবক DriveChunkWire &other) ধ্রুবক
	{
		যদি (wire != other.wire)
			ফেরত wire->name < other.wire->name;
		যদি (width != other.width)
			ফেরত width < other.width;
		ফেরত offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;

	explicit operator SigChunk() ধ্রুবক
	{
		ফেরত SigChunk(wire, offset, width);
	}
};

কাঠামো DriveChunkPort
{
	Cell *cell;
	IdString port;
	পূর্ণসংখ্যা offset;
	পূর্ণসংখ্যা width;

	DriveChunkPort(Cell *cell, IdString port, পূর্ণসংখ্যা offset, পূর্ণসংখ্যা width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(Get_আকার(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> ধ্রুবক &conn) :
		cell(cell), port(conn.first), offset(0), width(Get_আকার(conn.second)) { }
	DriveChunkPort(DriveBitPort ধ্রুবক &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত width; }

	DriveBitPort operator[](পূর্ণসংখ্যা i) ধ্রুবক
	{
		log_assert(i >= 0 && i < width);
		ফেরত DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort ধ্রুবক &bit) ধ্রুবক;
	bool try_append(DriveBitPort ধ্রুবক &bit);
	bool try_append(DriveChunkPort ধ্রুবক &chunk);

	// Whether this chunk is a whole port
	bool is_whole() ধ্রুবক
	{
		ফেরত offset == 0 && width == cell->connections().at(port).আকার_2();
	}

	bool operator==(ধ্রুবক DriveChunkPort &other) ধ্রুবক
	{
		ফেরত cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(ধ্রুবক DriveChunkPort &other) ধ্রুবক
	{
		যদি (cell != other.cell)
			ফেরত cell->name < other.cell->name;
		যদি (port != other.port)
			ফেরত port < other.port;
		যদি (width != other.width)
			ফেরত width < other.width;
		ফেরত offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;
};


কাঠামো DriveChunkMarker
{
	পূর্ণসংখ্যা marker;
	পূর্ণসংখ্যা offset;
	পূর্ণসংখ্যা width;

	DriveChunkMarker(পূর্ণসংখ্যা marker, পূর্ণসংখ্যা offset, পূর্ণসংখ্যা width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker ধ্রুবক &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত width; }

	DriveBitMarker operator[](পূর্ণসংখ্যা i) ধ্রুবক
	{
		log_assert(i >= 0 && i < width);
		ফেরত DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker ধ্রুবক &bit) ধ্রুবক;
	bool try_append(DriveBitMarker ধ্রুবক &bit);
	bool try_append(DriveChunkMarker ধ্রুবক &chunk);

	bool operator==(ধ্রুবক DriveChunkMarker &other) ধ্রুবক
	{
		ফেরত marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(ধ্রুবক DriveChunkMarker &other) ধ্রুবক
	{
		যদি (marker != other.marker)
			ফেরত marker < other.marker;
		যদি (width != other.width)
			ফেরত width < other.width;
		ফেরত offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;
};

কাঠামো DriveChunkMultiple
{
ব্যক্তিগত:
	mutable pool<DriveChunk> multiple_;
	পূর্ণসংখ্যা width_;

সর্বজনীন:
	pool<DriveChunk> ধ্রুবক &multiple() ধ্রুবক { ফেরত multiple_; }

	DriveChunkMultiple(DriveBitMultiple ধ্রুবক &bit);

	পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত width_; }

	DriveBitMultiple operator[](পূর্ণসংখ্যা i) ধ্রুবক;

	bool can_append(DriveBitMultiple ধ্রুবক &bit) ধ্রুবক;

	bool try_append(DriveBitMultiple ধ্রুবক &bit);


	bool can_append(DriveChunkMultiple ধ্রুবক &bit) ধ্রুবক;

	bool try_append(DriveChunkMultiple ধ্রুবক &bit);

	bool operator==(ধ্রুবক DriveChunkMultiple &other) ধ্রুবক
	{
		ফেরত width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(ধ্রুবক DriveChunkMultiple &other) ধ্রুবক
	{
		যদি (multiple_.আকার_2() < other.multiple_.আকার_2())

		multiple_.sort();
		ফেরত false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;
};

কাঠামো DriveChunk
{
ব্যক্তিগত:
	DriveType type_ = DriveType::NONE;
	সংঘ
	{
		পূর্ণসংখ্যা none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

সর্বজনীন:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk ধ্রুবক &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit ধ্রুবক &other) { *this = other; }

	DriveChunk(Const ধ্রুবক &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ধ্রুবক &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort ধ্রুবক &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker ধ্রুবক &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple ধ্রুবক &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](পূর্ণসংখ্যা i) ধ্রুবক
	{
		নির্বাচন (type_)
		{
			ক্ষেত্র DriveType::NONE:
				ফেরত DriveBit();
			ক্ষেত্র DriveType::CONSTANT:
				ফেরত constant_[i];
			ক্ষেত্র DriveType::WIRE:
				ফেরত wire_[i];
			ক্ষেত্র DriveType::PORT:
				ফেরত port_[i];
			ক্ষেত্র DriveType::MARKER:
				ফেরত marker_[i];
			ক্ষেত্র DriveType::MULTIPLE:
				ফেরত multiple_[i];
		}
		log_abort();
	}

	শূন্য set_none(পূর্ণসংখ্যা width = 0)
	{
		নির্বাচন (type_)
		{
			ক্ষেত্র DriveType::NONE:
				none_ = width;
				বিরতি;
			ক্ষেত্র DriveType::CONSTANT:
				constant_.~Const();
				বিরতি;
			ক্ষেত্র DriveType::WIRE:
				wire_.~DriveChunkWire();
				বিরতি;
			ক্ষেত্র DriveType::PORT:
				port_.~DriveChunkPort();
				বিরতি;
			ক্ষেত্র DriveType::MARKER:
				marker_.~DriveChunkMarker();
				বিরতি;
			ক্ষেত্র DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				বিরতি;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk ধ্রুবক &other)
	{
		নির্বাচন (other.type_)
		{
			ক্ষেত্র DriveType::NONE:
				set_none(other.none_);
				বিরতি;
			ক্ষেত্র DriveType::CONSTANT:
				*this = other.constant_;
				বিরতি;
			ক্ষেত্র DriveType::WIRE:
				*this = other.wire_;
				বিরতি;
			ক্ষেত্র DriveType::PORT:
				*this = other.port_;
				বিরতি;
			ক্ষেত্র DriveType::MARKER:
				*this = other.marker_;
				বিরতি;
			ক্ষেত্র DriveType::MULTIPLE:
				*this = other.multiple_;
				বিরতি;
		}
		ফেরত *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		নির্বাচন (other.type_)
		{
			ক্ষেত্র DriveType::NONE:
				set_none(other.none_);
				বিরতি;
			ক্ষেত্র DriveType::CONSTANT:
				*this = std::move(other.constant_);
				বিরতি;
			ক্ষেত্র DriveType::WIRE:
				*this = std::move(other.wire_);
				বিরতি;
			ক্ষেত্র DriveType::PORT:
				*this = std::move(other.port_);
				বিরতি;
			ক্ষেত্র DriveType::MARKER:
				*this = std::move(other.marker_);
				বিরতি;
			ক্ষেত্র DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				বিরতি;
		}
		ফেরত *this;
	}

	DriveChunk &operator=(Const ধ্রুবক &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		ফেরত *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		ফেরত *this;
	}

	DriveChunk &operator=(DriveChunkWire ধ্রুবক &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ফেরত *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ফেরত *this;
	}

	DriveChunk &operator=(DriveChunkPort ধ্রুবক &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ফেরত *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ফেরত *this;
	}

	DriveChunk &operator=(DriveChunkMarker ধ্রুবক &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ফেরত *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ফেরত *this;
	}

	DriveChunk &operator=(DriveChunkMultiple ধ্রুবক &multiple)
	{
		set_none(multiple.আকার_2());
		যদি (multiple.multiple().empty())
			ফেরত *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ফেরত *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.আকার_2());
		যদি (multiple.multiple().empty())
			ফেরত *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ফেরত *this;
	}

	DriveChunk &operator=(DriveBit ধ্রুবক &other)
	{
		নির্বাচন (other.type())
		{
			ক্ষেত্র DriveType::NONE:
				set_none(1);
				বিরতি;
			ক্ষেত্র DriveType::CONSTANT:
				*this = Const(other.constant());
				বিরতি;
			ক্ষেত্র DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				বিরতি;
			ক্ষেত্র DriveType::PORT:
				*this = DriveChunkPort(other.port());
				বিরতি;
			ক্ষেত্র DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				বিরতি;
			ক্ষেত্র DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				বিরতি;
		}
		ফেরত *this;
	}

	bool can_append(DriveBit ধ্রুবক &bit) ধ্রুবক;
	bool try_append(DriveBit ধ্রুবক &bit);
	bool try_append(DriveChunk ধ্রুবক &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;

	bool operator==(ধ্রুবক DriveChunk &other) ধ্রুবক
	{
		যদি (type_ != other.type_)
			ফেরত false;

		নির্বাচন (type_)
		{
			ক্ষেত্র DriveType::NONE:
				ফেরত true;
			ক্ষেত্র DriveType::CONSTANT:
				ফেরত constant_ == other.constant_;
			ক্ষেত্র DriveType::WIRE:
				ফেরত wire_ == other.wire_;
			ক্ষেত্র DriveType::PORT:
				ফেরত port_ == other.port_;
			ক্ষেত্র DriveType::MARKER:
				ফেরত marker_ == other.marker_;
			ক্ষেত্র DriveType::MULTIPLE:
				ফেরত multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ধ্রুবক DriveChunk &other) ধ্রুবক
	{
		ফেরত !(*this == other);
	}

	bool operator<(ধ্রুবক DriveChunk &other) ধ্রুবক
	{
		যদি (type_ != other.type_)
			ফেরত type_ < other.type_;
		নির্বাচন (type_)
		{
			ক্ষেত্র DriveType::NONE:
				ফেরত false;
			ক্ষেত্র DriveType::CONSTANT:
				ফেরত constant_ < other.constant_;
			ক্ষেত্র DriveType::WIRE:
				ফেরত wire_ < other.wire_;
			ক্ষেত্র DriveType::PORT:
				ফেরত port_ < other.port_;
			ক্ষেত্র DriveType::MARKER:
				ফেরত marker_ < other.marker_;
			ক্ষেত্র DriveType::MULTIPLE:
				ফেরত multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() ধ্রুবক { ফেরত type_; }

	bool is_none() ধ্রুবক { ফেরত type_ == DriveType::NONE; }
	bool is_constant() ধ্রুবক { ফেরত type_ == DriveType::CONSTANT; }
	bool is_wire() ধ্রুবক { ফেরত type_ == DriveType::WIRE; }
	bool is_port() ধ্রুবক { ফেরত type_ == DriveType::PORT; }
	bool is_marker() ধ্রুবক { ফেরত type_ == DriveType::MARKER; }
	bool is_multiple() ধ্রুবক { ফেরত type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); ফেরত constant_; }
	Const ধ্রুবক &constant() ধ্রুবক { log_assert(is_constant()); ফেরত constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); ফেরত wire_; }
	DriveChunkWire ধ্রুবক &wire() ধ্রুবক { log_assert(is_wire()); ফেরত wire_; }
	DriveChunkPort &port() { log_assert(is_port()); ফেরত port_; }
	DriveChunkPort ধ্রুবক &port() ধ্রুবক { log_assert(is_port()); ফেরত port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); ফেরত marker_; }
	DriveChunkMarker ধ্রুবক &marker() ধ্রুবক { log_assert(is_marker()); ফেরত marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); ফেরত multiple_; }
	DriveChunkMultiple ধ্রুবক &multiple() ধ্রুবক { log_assert(is_multiple()); ফেরত multiple_; }


	পূর্ণসংখ্যা আকার_2() ধ্রুবক
	{
		নির্বাচন (type_)
		{
			ক্ষেত্র DriveType::NONE:
				ফেরত none_;
			ক্ষেত্র DriveType::CONSTANT:
				ফেরত constant_.আকার_2();
			ক্ষেত্র DriveType::WIRE:
				ফেরত wire_.আকার_2();
			ক্ষেত্র DriveType::PORT:
				ফেরত port_.আকার_2();
			ক্ষেত্র DriveType::MARKER:
				ফেরত marker_.আকার_2();
			ক্ষেত্র DriveType::MULTIPLE:
				ফেরত multiple_.আকার_2();
		}
		log_abort();
	}
};

অন্তঃরেখ DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple ধ্রুবক &bit)
	: width_(1)
{
	জন্য (auto ধ্রুবক &bit : bit.multiple())
		multiple_.emplace(bit);
}

কাঠামো DriveSpec
{
ব্যক্তিগত:
	পূর্ণসংখ্যা width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned পূর্ণসংখ্যা hash_ = 0;
সর্বজনীন:

	অন্তঃরেখ bool packed() ধ্রুবক {
		ফেরত bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ধ্রুবক &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ধ্রুবক &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort ধ্রুবক &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ধ্রুবক &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple ধ্রুবক &chunk) { *this = chunk; }

	DriveSpec(DriveBit ধ্রুবক &bit) { *this = bit; }
	DriveSpec(DriveBitWire ধ্রুবক &bit) { *this = bit; }
	DriveSpec(DriveBitPort ধ্রুবক &bit) { *this = bit; }
	DriveSpec(DriveBitMarker ধ্রুবক &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple ধ্রুবক &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> ধ্রুবক &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> ধ্রুবক &bits)
	{
		জন্য (auto ধ্রুবক &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec ধ্রুবক &sig)
	{
		// TODO: converting one chunk at a time would be faster
		জন্য (auto ধ্রুবক &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> ধ্রুবক &chunks() ধ্রুবক { pack(); ফেরত chunks_; }
	std::vector<DriveBit> ধ্রুবক &bits() ধ্রুবক { unpack(); ফেরত bits_; }

	পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত width_; }

	শূন্য append(DriveBit ধ্রুবক &bit);

	শূন্য append(DriveChunk ধ্রুবক &chunk);

	শূন্য pack() ধ্রুবক;

	শূন্য unpack() ধ্রুবক;

	DriveBit &operator[](পূর্ণসংখ্যা সূচক)
	{
		log_assert(সূচক >= 0 && সূচক < আকার_2());
		unpack();
		ফেরত bits_[সূচক];
	}

	ধ্রুবক DriveBit &operator[](পূর্ণসংখ্যা সূচক) ধ্রুবক
	{
		log_assert(সূচক >= 0 && সূচক < আকার_2());
		unpack();
		ফেরত bits_[সূচক];
	}

	শূন্য clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk ধ্রুবক &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		ফেরত *this;
	}

	DriveSpec &operator=(DriveChunkWire ধ্রুবক &chunk) { ফেরত *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort ধ্রুবক &chunk) { ফেরত *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker ধ্রুবক &chunk) { ফেরত *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple ধ্রুবক &chunk) { ফেরত *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit ধ্রুবক &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		ফেরত *this;
	}

	DriveSpec &operator=(DriveBitWire ধ্রুবক &bit) { ফেরত *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort ধ্রুবক &bit) { ফেরত *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker ধ্রুবক &bit) { ফেরত *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple ধ্রুবক &bit) { ফেরত *this = DriveBit(bit); }

	শূন্য updhash() ধ্রুবক {
		যদি (hash_ != 0)
			ফেরত;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;

	bool operator==(DriveSpec ধ্রুবক &other) ধ্রুবক {
		updhash();
		other.updhash();
		যদি (আকার_2() != other.আকার_2() || hash_ != other.hash_)
			ফেরত false;
		ফেরত chunks() == other.chunks();
	}

ব্যক্তিগত:
	শূন্য compute_width();
};



কাঠামো DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

ব্যক্তিগত:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	কাঠামো DriveBitId
	{
		পূর্ণসংখ্যা id = -1;

		DriveBitId() {};

		DriveBitId(পূর্ণসংখ্যা id) : id(id) { }

		bool operator==(ধ্রুবক DriveBitId &other) ধ্রুবক { ফেরত id == other.id; }
		bool operator!=(ধ্রুবক DriveBitId &other) ধ্রুবক { ফেরত id != other.id; }
		bool operator<(ধ্রুবক DriveBitId &other) ধ্রুবক { ফেরত id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	কাঠামো DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		শূন্য add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId বের_করো_edge(DriveBitId src);
		শূন্য clear(DriveBitId src);
		bool contains(DriveBitId src);
		পূর্ণসংখ্যা গুনতি(DriveBitId src);

		DriveBitId at(DriveBitId src, পূর্ণসংখ্যা সূচক);
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
	পূর্ণসংখ্যা পরবর্তী_offset = 1 + (পূর্ণসংখ্যা)State::Sm;

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

	BitMode bit_mode(DriveBit ধ্রুবক &bit);
	DriveBitId id_from_drive_bit(DriveBit ধ্রুবক &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	শূন্য connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	শূন্য connect_directed_বাফার(DriveBitId driven_id, DriveBitId driver_id);
	শূন্য connect_undirected(DriveBitId a_id, DriveBitId b_id);

সর্বজনীন:

	শূন্য add(Module *module);

	// Add a single bit connection to the driver map.
	শূন্য add(DriveBit ধ্রুবক &a, DriveBit ধ্রুবক &b);

	template<typename T>
	স্থির constexpr bool is_sig_type() {
		ফেরত
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
	add(T ধ্রুবক &a, U ধ্রুবক &b)
	{
		log_assert(a.আকার_2() == b.আকার_2());
		জন্য (পূর্ণসংখ্যা i = 0; i != Get_আকার(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	শূন্য add(SigSpec ধ্রুবক &a, SigSpec ধ্রুবক &b);

ব্যক্তিগত:
	শূন্য add_port(Cell *cell, IdString port, SigSpec ধ্রুবক &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, পূর্ণসংখ্যা> orient_undirected_distance;

	শূন্য orient_undirected(DriveBitId id);

সর্বজনীন:
	DriveBit operator()(DriveBit ধ্রুবক &bit);

	DriveSpec operator()(DriveSpec spec);

ব্যক্তিগত:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		ফেরত wire->has_attribute(ID(keep));
	}
};

অন্তঃরেখ Hasher DriveBitWire::hash_into(Hasher h) ধ্রুবক
{
	h.eat(wire->name);
	h.eat(offset);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveBitPort::hash_into(Hasher h) ধ্রুবক
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveBitMarker::hash_into(Hasher h) ধ্রুবক
{
	h.eat(marker);
	h.eat(offset);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveBitMultiple::hash_into(Hasher h) ধ্রুবক
{
	h.eat(multiple_);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveBit::hash_into(Hasher h) ধ্রুবক
{
	নির্বাচন (type_) {
	ক্ষেত্র DriveType::NONE:
		h.eat(0);
		বিরতি;
	ক্ষেত্র DriveType::CONSTANT:
		h.eat(constant_);
		বিরতি;
	ক্ষেত্র DriveType::WIRE:
		h.eat(wire_);
		বিরতি;
	ক্ষেত্র DriveType::PORT:
		h.eat(port_);
		বিরতি;
	ক্ষেত্র DriveType::MARKER:
		h.eat(marker_);
		বিরতি;
	ক্ষেত্র DriveType::MULTIPLE:
		h.eat(multiple_);
		বিরতি;
	}
	h.eat(type_);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveChunkWire::hash_into(Hasher h) ধ্রুবক
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveChunkPort::hash_into(Hasher h) ধ্রুবক
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveChunkMarker::hash_into(Hasher h) ধ্রুবক
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveChunkMultiple::hash_into(Hasher h) ধ্রুবক
{
	h.eat(width_);
	h.eat(multiple_);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveChunk::hash_into(Hasher h) ধ্রুবক
{
	নির্বাচন (type_) {
	ক্ষেত্র DriveType::NONE:
		h.eat(0);
		বিরতি;
	ক্ষেত্র DriveType::CONSTANT:
		h.eat(constant_);
		বিরতি;
	ক্ষেত্র DriveType::WIRE:
		h.eat(wire_);
		বিরতি;
	ক্ষেত্র DriveType::PORT:
		h.eat(port_);
		বিরতি;
	ক্ষেত্র DriveType::MARKER:
		h.eat(marker_);
		বিরতি;
	ক্ষেত্র DriveType::MULTIPLE:
		h.eat(multiple_);
		বিরতি;
	}
	h.eat(type_);
	ফেরত h;
}

অন্তঃরেখ Hasher DriveSpec::hash_into(Hasher h) ধ্রুবক
{
	updhash();
	h.eat(hash_);
	ফেরত h;
}

অন্তঃরেখ Hasher DriverMap::DriveBitId::hash_into(Hasher h) ধ্রুবক
{
	h.eat(id);
	ফেরত h;
}

YOSYS_NAMESPACE_END

#endif
