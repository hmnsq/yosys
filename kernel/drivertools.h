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

#ଅନ୍ତର୍ଭାଵ <string>
#ଅନ୍ତର୍ଭାଵ <type_traits>

#ଅନ୍ତର୍ଭାଵ "kernel/rtlil.h"
#ଅନ୍ତର୍ଭାଵ "kernel/sigtools.h"
#ଅନ୍ତର୍ଭାଵ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

ସଂରଚନା DriveBit;

ସଂରଚନା DriveChunkWire;
ସଂରଚନା DriveChunkPort;
ସଂରଚନା DriveChunkMarker;
ସଂରଚନା DriveChunk;

ସଂରଚନା DriveSpec;

std::string log_signal(DriveChunkWire ଅଚଳ &chunk);
std::string log_signal(DriveChunkPort ଅଚଳ &chunk);
std::string log_signal(DriveChunkMarker ଅଚଳ &chunk);
std::string log_signal(DriveChunk ଅଚଳ &chunk);
std::string log_signal(DriveSpec ଅଚଳ &chunk);

ଗଣନା class DriveType : unsigned ଅକ୍ଷର
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

ସଂରଚନା DriveBitWire
{
	Wire *wire;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;

	DriveBitWire(Wire *wire, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) : wire(wire), offset(offset) {}

	bool operator==(ଅଚଳ DriveBitWire &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ wire == other.wire && offset == other.offset;
	}

	bool operator<(ଅଚଳ DriveBitWire &other) ଅଚଳ
	{
		ଯଦି (wire != other.wire)
			ଫେରନ୍ତୁ wire->name < other.wire->name;
		ଫେରନ୍ତୁ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;


	operator SigBit() ଅଚଳ
	{
		ଫେରନ୍ତୁ SigBit(wire, offset);
	}
};

ସଂରଚନା DriveBitPort
{
	Cell *cell;
	IdString port;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;

	DriveBitPort(Cell *cell, IdString port, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(ଅଚଳ DriveBitPort &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(ଅଚଳ DriveBitPort &other) ଅଚଳ
	{
		ଯଦି (cell != other.cell)
			ଫେରନ୍ତୁ cell->name < other.cell->name;
		ଯଦି (port != other.port)
			ଫେରନ୍ତୁ port < other.port;
		ଫେରନ୍ତୁ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;

};


ସଂରଚନା DriveBitMarker
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା marker;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;

	DriveBitMarker(ପୂର୍ଣ୍ଣସଂଖ୍ୟା marker, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) : marker(marker), offset(offset) {}

	bool operator==(ଅଚଳ DriveBitMarker &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ marker == other.marker && offset == other.offset;
	}

	bool operator<(ଅଚଳ DriveBitMarker &other) ଅଚଳ
	{
		ଯଦି (marker != other.marker)
			ଫେରନ୍ତୁ marker < other.marker;
		ଫେରନ୍ତୁ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;

};

ସଂରଚନା DriveBitMultiple
{
ବ୍ୟକ୍ତିଗତ:
	pool<DriveBit> multiple_;

ସର୍ବସାଧାରଣ:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit ଅଚଳ &single);

	pool<DriveBit> ଅଚଳ &multiple() ଅଚଳ { ଫେରନ୍ତୁ multiple_; }

	ଶୂନ୍ୟ merge(DriveBitMultiple ଅଚଳ &other)
	{
		ପାଇଁ (DriveBit ଅଚଳ &single : other.multiple_)
			merge(single);
	}

	ଶୂନ୍ୟ merge(DriveBitMultiple &&other)
	{
		ପାଇଁ (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	ଶୂନ୍ୟ merge(DriveBit ଅଚଳ &single);
	ଶୂନ୍ୟ merge(DriveBit &&single);

	bool operator==(ଅଚଳ DriveBitMultiple &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;
};

ସଂରଚନା DriveBit
{
ବ୍ୟକ୍ତିଗତ:
	DriveType type_ = DriveType::NONE;
	ସଂଘ
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
ସର୍ବସାଧାରଣ:
	DriveBit()  {}

	DriveBit(SigBit ଅଚଳ &bit);

	DriveBit(DriveBit ଅଚଳ &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire ଅଚଳ &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort ଅଚଳ &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker ଅଚଳ &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple ଅଚଳ &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	ଶୂନ୍ୟ set_none()
	{
		ବାଛ (type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::CONSTANT:
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE:
				wire_.~DriveBitWire();
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT:
				port_.~DriveBitPort();
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER:
				marker_.~DriveBitMarker();
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				ଭାଙ୍ଗ;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit ଅଚଳ &other)
	{
		ବାଛ (other.type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				set_none();
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::CONSTANT:
				*this = other.constant_;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE:
				*this = other.wire_;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT:
				*this = other.port_;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER:
				*this = other.marker_;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MULTIPLE:
				*this = other.multiple_;
				ଭାଙ୍ଗ;
		}
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		ବାଛ (other.type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				set_none();
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE:
				*this = std::move(other.wire_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT:
				*this = std::move(other.port_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER:
				*this = std::move(other.marker_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				ଭାଙ୍ଗ;
		}
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(DriveBitWire ଅଚଳ &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(DriveBitPort ଅଚଳ &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(DriveBitMarker ଅଚଳ &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(DriveBitMultiple ଅଚଳ &multiple)
	{
		set_none();
		ଯଦି (multiple.multiple().empty())
			ଫେରନ୍ତୁ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ଫେରନ୍ତୁ *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		ଯଦି (multiple.multiple().empty())
			ଫେରନ୍ତୁ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ଫେରନ୍ତୁ *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;

	bool operator==(ଅଚଳ DriveBit &other) ଅଚଳ
	{
		ଯଦି (type_ != other.type_)
			ଫେରନ୍ତୁ false;

		ବାଛ (type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				ଫେରନ୍ତୁ true;
			ସ୍ଥିତି DriveType::CONSTANT:
				ଫେରନ୍ତୁ constant_ == other.constant_;
			ସ୍ଥିତି DriveType::WIRE:
				ଫେରନ୍ତୁ wire_ == other.wire_;
			ସ୍ଥିତି DriveType::PORT:
				ଫେରନ୍ତୁ port_ == other.port_;
			ସ୍ଥିତି DriveType::MARKER:
				ଫେରନ୍ତୁ marker_ == other.marker_;
			ସ୍ଥିତି DriveType::MULTIPLE:
				ଫେରନ୍ତୁ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ଅଚଳ DriveBit &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ !(*this == other);
	}

	bool operator<(ଅଚଳ DriveBit &other) ଅଚଳ
	{
		ଯଦି (type_ != other.type_)
			ଫେରନ୍ତୁ type_ < other.type_;
		ବାଛ (type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				ଫେରନ୍ତୁ false;
			ସ୍ଥିତି DriveType::CONSTANT:
				ଫେରନ୍ତୁ constant_ < other.constant_;
			ସ୍ଥିତି DriveType::WIRE:
				ଫେରନ୍ତୁ wire_ < other.wire_;
			ସ୍ଥିତି DriveType::PORT:
				ଫେରନ୍ତୁ port_ < other.port_;
			ସ୍ଥିତି DriveType::MARKER:
				ଫେରନ୍ତୁ marker_ < other.marker_;
			ସ୍ଥିତି DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ଅଚଳ { ଫେରନ୍ତୁ type_; }

	bool is_none() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::NONE; }
	bool is_constant() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::CONSTANT; }
	bool is_wire() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::WIRE; }
	bool is_port() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::PORT; }
	bool is_marker() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::MARKER; }
	bool is_multiple() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); ଫେରନ୍ତୁ constant_; }
	State ଅଚଳ &constant() ଅଚଳ { log_assert(is_constant()); ଫେରନ୍ତୁ constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); ଫେରନ୍ତୁ wire_; }
	DriveBitWire ଅଚଳ &wire() ଅଚଳ { log_assert(is_wire()); ଫେରନ୍ତୁ wire_; }
	DriveBitPort &port() { log_assert(is_port()); ଫେରନ୍ତୁ port_; }
	DriveBitPort ଅଚଳ &port() ଅଚଳ { log_assert(is_port()); ଫେରନ୍ତୁ port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); ଫେରନ୍ତୁ marker_; }
	DriveBitMarker ଅଚଳ &marker() ଅଚଳ { log_assert(is_marker()); ଫେରନ୍ତୁ marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); ଫେରନ୍ତୁ multiple_; }
	DriveBitMultiple ଅଚଳ &multiple() ଅଚଳ { log_assert(is_multiple()); ଫେରନ୍ତୁ multiple_; }

	ଶୂନ୍ୟ merge(DriveBit ଅଚଳ &other);

};

ଧାଡ଼ିଭିତରେ DriveBitMultiple::DriveBitMultiple() {}
ଧାଡ଼ିଭିତରେ DriveBitMultiple::DriveBitMultiple(DriveBit ଅଚଳ &single)
{
	multiple_.emplace(single);
}

ସଂରଚନା DriveChunkWire
{
	Wire *wire;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;

	DriveChunkWire(Wire *wire, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire ଅଚଳ &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ width; }

	DriveBitWire operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) ଅଚଳ
	{
		log_assert(i >= 0 && i < width);
		ଫେରନ୍ତୁ DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire ଅଚଳ &bit) ଅଚଳ;
	bool try_append(DriveBitWire ଅଚଳ &bit);
	bool try_append(DriveChunkWire ଅଚଳ &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() ଅଚଳ { ଫେରନ୍ତୁ offset == 0 && width == wire->width; }

	bool operator==(ଅଚଳ DriveChunkWire &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(ଅଚଳ DriveChunkWire &other) ଅଚଳ
	{
		ଯଦି (wire != other.wire)
			ଫେରନ୍ତୁ wire->name < other.wire->name;
		ଯଦି (width != other.width)
			ଫେରନ୍ତୁ width < other.width;
		ଫେରନ୍ତୁ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;

	explicit operator SigChunk() ଅଚଳ
	{
		ଫେରନ୍ତୁ SigChunk(wire, offset, width);
	}
};

ସଂରଚନା DriveChunkPort
{
	Cell *cell;
	IdString port;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;

	DriveChunkPort(Cell *cell, IdString port, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> ଅଚଳ &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort ଅଚଳ &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ width; }

	DriveBitPort operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) ଅଚଳ
	{
		log_assert(i >= 0 && i < width);
		ଫେରନ୍ତୁ DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort ଅଚଳ &bit) ଅଚଳ;
	bool try_append(DriveBitPort ଅଚଳ &bit);
	bool try_append(DriveChunkPort ଅଚଳ &chunk);

	// Whether this chunk is a whole port
	bool is_whole() ଅଚଳ
	{
		ଫେରନ୍ତୁ offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(ଅଚଳ DriveChunkPort &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(ଅଚଳ DriveChunkPort &other) ଅଚଳ
	{
		ଯଦି (cell != other.cell)
			ଫେରନ୍ତୁ cell->name < other.cell->name;
		ଯଦି (port != other.port)
			ଫେରନ୍ତୁ port < other.port;
		ଯଦି (width != other.width)
			ଫେରନ୍ତୁ width < other.width;
		ଫେରନ୍ତୁ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;
};


ସଂରଚନା DriveChunkMarker
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା marker;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;

	DriveChunkMarker(ପୂର୍ଣ୍ଣସଂଖ୍ୟା marker, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker ଅଚଳ &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ width; }

	DriveBitMarker operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) ଅଚଳ
	{
		log_assert(i >= 0 && i < width);
		ଫେରନ୍ତୁ DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker ଅଚଳ &bit) ଅଚଳ;
	bool try_append(DriveBitMarker ଅଚଳ &bit);
	bool try_append(DriveChunkMarker ଅଚଳ &chunk);

	bool operator==(ଅଚଳ DriveChunkMarker &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(ଅଚଳ DriveChunkMarker &other) ଅଚଳ
	{
		ଯଦି (marker != other.marker)
			ଫେରନ୍ତୁ marker < other.marker;
		ଯଦି (width != other.width)
			ଫେରନ୍ତୁ width < other.width;
		ଫେରନ୍ତୁ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;
};

ସଂରଚନା DriveChunkMultiple
{
ବ୍ୟକ୍ତିଗତ:
	mutable pool<DriveChunk> multiple_;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width_;

ସର୍ବସାଧାରଣ:
	pool<DriveChunk> ଅଚଳ &multiple() ଅଚଳ { ଫେରନ୍ତୁ multiple_; }

	DriveChunkMultiple(DriveBitMultiple ଅଚଳ &bit);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ width_; }

	DriveBitMultiple operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) ଅଚଳ;

	bool can_append(DriveBitMultiple ଅଚଳ &bit) ଅଚଳ;

	bool try_append(DriveBitMultiple ଅଚଳ &bit);


	bool can_append(DriveChunkMultiple ଅଚଳ &bit) ଅଚଳ;

	bool try_append(DriveChunkMultiple ଅଚଳ &bit);

	bool operator==(ଅଚଳ DriveChunkMultiple &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(ଅଚଳ DriveChunkMultiple &other) ଅଚଳ
	{
		ଯଦି (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		ଫେରନ୍ତୁ false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;
};

ସଂରଚନା DriveChunk
{
ବ୍ୟକ୍ତିଗତ:
	DriveType type_ = DriveType::NONE;
	ସଂଘ
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

ସର୍ବସାଧାରଣ:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk ଅଚଳ &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit ଅଚଳ &other) { *this = other; }

	DriveChunk(Const ଅଚଳ &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ଅଚଳ &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort ଅଚଳ &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker ଅଚଳ &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple ଅଚଳ &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) ଅଚଳ
	{
		ବାଛ (type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				ଫେରନ୍ତୁ DriveBit();
			ସ୍ଥିତି DriveType::CONSTANT:
				ଫେରନ୍ତୁ constant_[i];
			ସ୍ଥିତି DriveType::WIRE:
				ଫେରନ୍ତୁ wire_[i];
			ସ୍ଥିତି DriveType::PORT:
				ଫେରନ୍ତୁ port_[i];
			ସ୍ଥିତି DriveType::MARKER:
				ଫେରନ୍ତୁ marker_[i];
			ସ୍ଥିତି DriveType::MULTIPLE:
				ଫେରନ୍ତୁ multiple_[i];
		}
		log_abort();
	}

	ଶୂନ୍ୟ set_none(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 0)
	{
		ବାଛ (type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				none_ = width;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::CONSTANT:
				constant_.~Const();
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE:
				wire_.~DriveChunkWire();
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT:
				port_.~DriveChunkPort();
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER:
				marker_.~DriveChunkMarker();
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				ଭାଙ୍ଗ;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk ଅଚଳ &other)
	{
		ବାଛ (other.type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				set_none(other.none_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::CONSTANT:
				*this = other.constant_;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE:
				*this = other.wire_;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT:
				*this = other.port_;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER:
				*this = other.marker_;
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MULTIPLE:
				*this = other.multiple_;
				ଭାଙ୍ଗ;
		}
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		ବାଛ (other.type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				set_none(other.none_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE:
				*this = std::move(other.wire_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT:
				*this = std::move(other.port_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER:
				*this = std::move(other.marker_);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				ଭାଙ୍ଗ;
		}
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(Const ଅଚଳ &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveChunkWire ଅଚଳ &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveChunkPort ଅଚଳ &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveChunkMarker ଅଚଳ &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveChunkMultiple ଅଚଳ &multiple)
	{
		set_none(multiple.size());
		ଯଦି (multiple.multiple().empty())
			ଫେରନ୍ତୁ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		ଯଦି (multiple.multiple().empty())
			ଫେରନ୍ତୁ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ଫେରନ୍ତୁ *this;
	}

	DriveChunk &operator=(DriveBit ଅଚଳ &other)
	{
		ବାଛ (other.type())
		{
			ସ୍ଥିତି DriveType::NONE:
				set_none(1);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::CONSTANT:
				*this = Const(other.constant());
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT:
				*this = DriveChunkPort(other.port());
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				ଭାଙ୍ଗ;
		}
		ଫେରନ୍ତୁ *this;
	}

	bool can_append(DriveBit ଅଚଳ &bit) ଅଚଳ;
	bool try_append(DriveBit ଅଚଳ &bit);
	bool try_append(DriveChunk ଅଚଳ &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;

	bool operator==(ଅଚଳ DriveChunk &other) ଅଚଳ
	{
		ଯଦି (type_ != other.type_)
			ଫେରନ୍ତୁ false;

		ବାଛ (type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				ଫେରନ୍ତୁ true;
			ସ୍ଥିତି DriveType::CONSTANT:
				ଫେରନ୍ତୁ constant_ == other.constant_;
			ସ୍ଥିତି DriveType::WIRE:
				ଫେରନ୍ତୁ wire_ == other.wire_;
			ସ୍ଥିତି DriveType::PORT:
				ଫେରନ୍ତୁ port_ == other.port_;
			ସ୍ଥିତି DriveType::MARKER:
				ଫେରନ୍ତୁ marker_ == other.marker_;
			ସ୍ଥିତି DriveType::MULTIPLE:
				ଫେରନ୍ତୁ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ଅଚଳ DriveChunk &other) ଅଚଳ
	{
		ଫେରନ୍ତୁ !(*this == other);
	}

	bool operator<(ଅଚଳ DriveChunk &other) ଅଚଳ
	{
		ଯଦି (type_ != other.type_)
			ଫେରନ୍ତୁ type_ < other.type_;
		ବାଛ (type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				ଫେରନ୍ତୁ false;
			ସ୍ଥିତି DriveType::CONSTANT:
				ଫେରନ୍ତୁ constant_ < other.constant_;
			ସ୍ଥିତି DriveType::WIRE:
				ଫେରନ୍ତୁ wire_ < other.wire_;
			ସ୍ଥିତି DriveType::PORT:
				ଫେରନ୍ତୁ port_ < other.port_;
			ସ୍ଥିତି DriveType::MARKER:
				ଫେରନ୍ତୁ marker_ < other.marker_;
			ସ୍ଥିତି DriveType::MULTIPLE:
				ଫେରନ୍ତୁ multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() ଅଚଳ { ଫେରନ୍ତୁ type_; }

	bool is_none() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::NONE; }
	bool is_constant() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::CONSTANT; }
	bool is_wire() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::WIRE; }
	bool is_port() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::PORT; }
	bool is_marker() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::MARKER; }
	bool is_multiple() ଅଚଳ { ଫେରନ୍ତୁ type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); ଫେରନ୍ତୁ constant_; }
	Const ଅଚଳ &constant() ଅଚଳ { log_assert(is_constant()); ଫେରନ୍ତୁ constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); ଫେରନ୍ତୁ wire_; }
	DriveChunkWire ଅଚଳ &wire() ଅଚଳ { log_assert(is_wire()); ଫେରନ୍ତୁ wire_; }
	DriveChunkPort &port() { log_assert(is_port()); ଫେରନ୍ତୁ port_; }
	DriveChunkPort ଅଚଳ &port() ଅଚଳ { log_assert(is_port()); ଫେରନ୍ତୁ port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); ଫେରନ୍ତୁ marker_; }
	DriveChunkMarker ଅଚଳ &marker() ଅଚଳ { log_assert(is_marker()); ଫେରନ୍ତୁ marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); ଫେରନ୍ତୁ multiple_; }
	DriveChunkMultiple ଅଚଳ &multiple() ଅଚଳ { log_assert(is_multiple()); ଫେରନ୍ତୁ multiple_; }


	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ
	{
		ବାଛ (type_)
		{
			ସ୍ଥିତି DriveType::NONE:
				ଫେରନ୍ତୁ none_;
			ସ୍ଥିତି DriveType::CONSTANT:
				ଫେରନ୍ତୁ constant_.size();
			ସ୍ଥିତି DriveType::WIRE:
				ଫେରନ୍ତୁ wire_.size();
			ସ୍ଥିତି DriveType::PORT:
				ଫେରନ୍ତୁ port_.size();
			ସ୍ଥିତି DriveType::MARKER:
				ଫେରନ୍ତୁ marker_.size();
			ସ୍ଥିତି DriveType::MULTIPLE:
				ଫେରନ୍ତୁ multiple_.size();
		}
		log_abort();
	}
};

ଧାଡ଼ିଭିତରେ DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple ଅଚଳ &bit)
	: width_(1)
{
	ପାଇଁ (auto ଅଚଳ &bit : bit.multiple())
		multiple_.emplace(bit);
}

ସଂରଚନା DriveSpec
{
ବ୍ୟକ୍ତିଗତ:
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hash_ = 0;
ସର୍ବସାଧାରଣ:

	ଧାଡ଼ିଭିତରେ bool packed() ଅଚଳ {
		ଫେରନ୍ତୁ bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ଅଚଳ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ଅଚଳ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort ଅଚଳ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ଅଚଳ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple ଅଚଳ &chunk) { *this = chunk; }

	DriveSpec(DriveBit ଅଚଳ &bit) { *this = bit; }
	DriveSpec(DriveBitWire ଅଚଳ &bit) { *this = bit; }
	DriveSpec(DriveBitPort ଅଚଳ &bit) { *this = bit; }
	DriveSpec(DriveBitMarker ଅଚଳ &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple ଅଚଳ &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> ଅଚଳ &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> ଅଚଳ &bits)
	{
		ପାଇଁ (auto ଅଚଳ &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec ଅଚଳ &sig)
	{
		// TODO: converting one chunk at a time would be faster
		ପାଇଁ (auto ଅଚଳ &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> ଅଚଳ &chunks() ଅଚଳ { pack(); ଫେରନ୍ତୁ chunks_; }
	std::vector<DriveBit> ଅଚଳ &bits() ଅଚଳ { unpack(); ଫେରନ୍ତୁ bits_; }

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ width_; }

	ଶୂନ୍ୟ append(DriveBit ଅଚଳ &bit);

	ଶୂନ୍ୟ append(DriveChunk ଅଚଳ &chunk);

	ଶୂନ୍ୟ pack() ଅଚଳ;

	ଶୂନ୍ୟ unpack() ଅଚଳ;

	DriveBit &operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		ଫେରନ୍ତୁ bits_[index];
	}

	ଅଚଳ DriveBit &operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ
	{
		log_assert(index >= 0 && index < size());
		unpack();
		ଫେରନ୍ତୁ bits_[index];
	}

	ଶୂନ୍ୟ clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk ଅଚଳ &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		ଫେରନ୍ତୁ *this;
	}

	DriveSpec &operator=(DriveChunkWire ଅଚଳ &chunk) { ଫେରନ୍ତୁ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort ଅଚଳ &chunk) { ଫେରନ୍ତୁ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker ଅଚଳ &chunk) { ଫେରନ୍ତୁ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple ଅଚଳ &chunk) { ଫେରନ୍ତୁ *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit ଅଚଳ &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		ଫେରନ୍ତୁ *this;
	}

	DriveSpec &operator=(DriveBitWire ଅଚଳ &bit) { ଫେରନ୍ତୁ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort ଅଚଳ &bit) { ଫେରନ୍ତୁ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker ଅଚଳ &bit) { ଫେରନ୍ତୁ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple ଅଚଳ &bit) { ଫେରନ୍ତୁ *this = DriveBit(bit); }

	ଶୂନ୍ୟ updhash() ଅଚଳ {
		ଯଦି (hash_ != 0)
			ଫେରନ୍ତୁ;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;

	bool operator==(DriveSpec ଅଚଳ &other) ଅଚଳ {
		updhash();
		other.updhash();
		ଯଦି (size() != other.size() || hash_ != other.hash_)
			ଫେରନ୍ତୁ false;
		ଫେରନ୍ତୁ chunks() == other.chunks();
	}

ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ compute_width();
};



ସଂରଚନା DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

ବ୍ୟକ୍ତିଗତ:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	ସଂରଚନା DriveBitId
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = -1;

		DriveBitId() {};

		DriveBitId(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) : id(id) { }

		bool operator==(ଅଚଳ DriveBitId &other) ଅଚଳ { ଫେରନ୍ତୁ id == other.id; }
		bool operator!=(ଅଚଳ DriveBitId &other) ଅଚଳ { ଫେରନ୍ତୁ id != other.id; }
		bool operator<(ଅଚଳ DriveBitId &other) ଅଚଳ { ଫେରନ୍ତୁ id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	ସଂରଚନା DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		ଶୂନ୍ୟ add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId କାଢ଼_edge(DriveBitId src);
		ଶୂନ୍ୟ clear(DriveBitId src);
		bool contains(DriveBitId src);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2(DriveBitId src);

		DriveBitId at(DriveBitId src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index);
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_offset = 1 + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)State::Sm;

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


	ଗଣନା class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit ଅଚଳ &bit);
	DriveBitId id_from_drive_bit(DriveBit ଅଚଳ &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	ଶୂନ୍ୟ connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	ଶୂନ୍ୟ connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	ଶୂନ୍ୟ connect_undirected(DriveBitId a_id, DriveBitId b_id);

ସର୍ବସାଧାରଣ:

	ଶୂନ୍ୟ add(Module *module);

	// Add a single bit connection to the driver map.
	ଶୂନ୍ୟ add(DriveBit ଅଚଳ &a, DriveBit ଅଚଳ &b);

	template<typename T>
	ସ୍ଥିର constexpr bool is_sig_type() {
		ଫେରନ୍ତୁ
			std::is_same<T, SigSpec>::ମୂଲ୍ୟ ||
			std::is_same<T, SigChunk>::ମୂଲ୍ୟ ||
			std::is_same<T, DriveSpec>::ମୂଲ୍ୟ ||
			std::is_same<T, DriveChunk>::ମୂଲ୍ୟ ||
			std::is_same<T, DriveChunkPort>::ମୂଲ୍ୟ ||
			std::is_same<T, DriveChunkWire>::ମୂଲ୍ୟ ||
			std::is_same<T, Const>::ମୂଲ୍ୟ;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T ଅଚଳ &a, U ଅଚଳ &b)
	{
		log_assert(a.size() == b.size());
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	ଶୂନ୍ୟ add(SigSpec ଅଚଳ &a, SigSpec ଅଚଳ &b);

ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ add_port(Cell *cell, IdString port, SigSpec ଅଚଳ &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> orient_undirected_distance;

	ଶୂନ୍ୟ orient_undirected(DriveBitId id);

ସର୍ବସାଧାରଣ:
	DriveBit operator()(DriveBit ଅଚଳ &bit);

	DriveSpec operator()(DriveSpec spec);

ବ୍ୟକ୍ତିଗତ:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		ଫେରନ୍ତୁ wire->has_attribute(ID(keep));
	}
};

ଧାଡ଼ିଭିତରେ Hasher DriveBitWire::hash_into(Hasher h) ଅଚଳ
{
	h.eat(wire->name);
	h.eat(offset);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveBitPort::hash_into(Hasher h) ଅଚଳ
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveBitMarker::hash_into(Hasher h) ଅଚଳ
{
	h.eat(marker);
	h.eat(offset);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveBitMultiple::hash_into(Hasher h) ଅଚଳ
{
	h.eat(multiple_);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveBit::hash_into(Hasher h) ଅଚଳ
{
	ବାଛ (type_) {
	ସ୍ଥିତି DriveType::NONE:
		h.eat(0);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::CONSTANT:
		h.eat(constant_);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::WIRE:
		h.eat(wire_);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::PORT:
		h.eat(port_);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::MARKER:
		h.eat(marker_);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::MULTIPLE:
		h.eat(multiple_);
		ଭାଙ୍ଗ;
	}
	h.eat(type_);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveChunkWire::hash_into(Hasher h) ଅଚଳ
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveChunkPort::hash_into(Hasher h) ଅଚଳ
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveChunkMarker::hash_into(Hasher h) ଅଚଳ
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveChunkMultiple::hash_into(Hasher h) ଅଚଳ
{
	h.eat(width_);
	h.eat(multiple_);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveChunk::hash_into(Hasher h) ଅଚଳ
{
	ବାଛ (type_) {
	ସ୍ଥିତି DriveType::NONE:
		h.eat(0);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::CONSTANT:
		h.eat(constant_);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::WIRE:
		h.eat(wire_);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::PORT:
		h.eat(port_);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::MARKER:
		h.eat(marker_);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି DriveType::MULTIPLE:
		h.eat(multiple_);
		ଭାଙ୍ଗ;
	}
	h.eat(type_);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriveSpec::hash_into(Hasher h) ଅଚଳ
{
	updhash();
	h.eat(hash_);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ Hasher DriverMap::DriveBitId::hash_into(Hasher h) ଅଚଳ
{
	h.eat(id);
	ଫେରନ୍ତୁ h;
}

YOSYS_NAMESPACE_END

#endif
