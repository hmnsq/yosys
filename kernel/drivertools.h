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

#include <string>
#include <type_traits>

#include "kernel/rtlil.h"
#include "kernel/sigtools.h"
#include "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

struct DriveBit;

struct DriveChunkWire;
struct DriveChunkPort;
struct DriveChunkMarker;
struct DriveChunk;

struct DriveSpec;

std::string log_signal(DriveChunkWire const &chunk);
std::string log_signal(DriveChunkPort const &chunk);
std::string log_signal(DriveChunkMarker const &chunk);
std::string log_signal(DriveChunk const &chunk);
std::string log_signal(DriveSpec const &chunk);

enum class DriveType : unsigned char
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

struct DriveBitWire
{
	Wire *wire;
	целое offset;

	DriveBitWire(Wire *wire, целое offset) : wire(wire), offset(offset) {}

	bool operator==(const DriveBitWire &other) const
	{
		вернуть wire == other.wire && offset == other.offset;
	}

	bool operator<(const DriveBitWire &other) const
	{
		если (wire != other.wire)
			вернуть wire->name < other.wire->name;
		вернуть offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;


	operator SigBit() const
	{
		вернуть SigBit(wire, offset);
	}
};

struct DriveBitPort
{
	Cell *cell;
	IdString port;
	целое offset;

	DriveBitPort(Cell *cell, IdString port, целое offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(const DriveBitPort &other) const
	{
		вернуть cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(const DriveBitPort &other) const
	{
		если (cell != other.cell)
			вернуть cell->name < other.cell->name;
		если (port != other.port)
			вернуть port < other.port;
		вернуть offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

};


struct DriveBitMarker
{
	целое marker;
	целое offset;

	DriveBitMarker(целое marker, целое offset) : marker(marker), offset(offset) {}

	bool operator==(const DriveBitMarker &other) const
	{
		вернуть marker == other.marker && offset == other.offset;
	}

	bool operator<(const DriveBitMarker &other) const
	{
		если (marker != other.marker)
			вернуть marker < other.marker;
		вернуть offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

};

struct DriveBitMultiple
{
private:
	pool<DriveBit> multiple_;

public:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit const &single);

	pool<DriveBit> const &multiple() const { вернуть multiple_; }

	void merge(DriveBitMultiple const &other)
	{
		для (DriveBit const &single : other.multiple_)
			merge(single);
	}

	void merge(DriveBitMultiple &&other)
	{
		для (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	void merge(DriveBit const &single);
	void merge(DriveBit &&single);

	bool operator==(const DriveBitMultiple &other) const
	{
		вернуть multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};

struct DriveBit
{
private:
	DriveType type_ = DriveType::NONE;
	union
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
public:
	DriveBit()  {}

	DriveBit(SigBit const &bit);

	DriveBit(DriveBit const &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire const &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort const &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker const &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple const &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	void set_none()
	{
		switch (type_)
		{
			case DriveType::NONE:
				break;
			case DriveType::CONSTANT:
				break;
			case DriveType::WIRE:
				wire_.~DriveBitWire();
				break;
			case DriveType::PORT:
				port_.~DriveBitPort();
				break;
			case DriveType::MARKER:
				marker_.~DriveBitMarker();
				break;
			case DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				break;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit const &other)
	{
		switch (other.type_)
		{
			case DriveType::NONE:
				set_none();
				break;
			case DriveType::CONSTANT:
				*this = other.constant_;
				break;
			case DriveType::WIRE:
				*this = other.wire_;
				break;
			case DriveType::PORT:
				*this = other.port_;
				break;
			case DriveType::MARKER:
				*this = other.marker_;
				break;
			case DriveType::MULTIPLE:
				*this = other.multiple_;
				break;
		}
		вернуть *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		switch (other.type_)
		{
			case DriveType::NONE:
				set_none();
				break;
			case DriveType::CONSTANT:
				*this = std::move(other.constant_);
				break;
			case DriveType::WIRE:
				*this = std::move(other.wire_);
				break;
			case DriveType::PORT:
				*this = std::move(other.port_);
				break;
			case DriveType::MARKER:
				*this = std::move(other.marker_);
				break;
			case DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				break;
		}
		вернуть *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		вернуть *this;
	}

	DriveBit &operator=(DriveBitWire const &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		вернуть *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		вернуть *this;
	}

	DriveBit &operator=(DriveBitPort const &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		вернуть *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		вернуть *this;
	}

	DriveBit &operator=(DriveBitMarker const &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		вернуть *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		вернуть *this;
	}

	DriveBit &operator=(DriveBitMultiple const &multiple)
	{
		set_none();
		если (multiple.multiple().empty())
			вернуть *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		вернуть *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		если (multiple.multiple().empty())
			вернуть *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		вернуть *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	bool operator==(const DriveBit &other) const
	{
		если (type_ != other.type_)
			вернуть false;

		switch (type_)
		{
			case DriveType::NONE:
				вернуть true;
			case DriveType::CONSTANT:
				вернуть constant_ == other.constant_;
			case DriveType::WIRE:
				вернуть wire_ == other.wire_;
			case DriveType::PORT:
				вернуть port_ == other.port_;
			case DriveType::MARKER:
				вернуть marker_ == other.marker_;
			case DriveType::MULTIPLE:
				вернуть multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(const DriveBit &other) const
	{
		вернуть !(*this == other);
	}

	bool operator<(const DriveBit &other) const
	{
		если (type_ != other.type_)
			вернуть type_ < other.type_;
		switch (type_)
		{
			case DriveType::NONE:
				вернуть false;
			case DriveType::CONSTANT:
				вернуть constant_ < other.constant_;
			case DriveType::WIRE:
				вернуть wire_ < other.wire_;
			case DriveType::PORT:
				вернуть port_ < other.port_;
			case DriveType::MARKER:
				вернуть marker_ < other.marker_;
			case DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() const { вернуть type_; }

	bool is_none() const { вернуть type_ == DriveType::NONE; }
	bool is_constant() const { вернуть type_ == DriveType::CONSTANT; }
	bool is_wire() const { вернуть type_ == DriveType::WIRE; }
	bool is_port() const { вернуть type_ == DriveType::PORT; }
	bool is_marker() const { вернуть type_ == DriveType::MARKER; }
	bool is_multiple() const { вернуть type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); вернуть constant_; }
	State const &constant() const { log_assert(is_constant()); вернуть constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); вернуть wire_; }
	DriveBitWire const &wire() const { log_assert(is_wire()); вернуть wire_; }
	DriveBitPort &port() { log_assert(is_port()); вернуть port_; }
	DriveBitPort const &port() const { log_assert(is_port()); вернуть port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); вернуть marker_; }
	DriveBitMarker const &marker() const { log_assert(is_marker()); вернуть marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); вернуть multiple_; }
	DriveBitMultiple const &multiple() const { log_assert(is_multiple()); вернуть multiple_; }

	void merge(DriveBit const &other);

};

inline DriveBitMultiple::DriveBitMultiple() {}
inline DriveBitMultiple::DriveBitMultiple(DriveBit const &single)
{
	multiple_.emplace(single);
}

struct DriveChunkWire
{
	Wire *wire;
	целое offset;
	целое width;

	DriveChunkWire(Wire *wire, целое offset, целое width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire const &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	целое size() const { вернуть width; }

	DriveBitWire operator[](целое i) const
	{
		log_assert(i >= 0 && i < width);
		вернуть DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire const &bit) const;
	bool try_append(DriveBitWire const &bit);
	bool try_append(DriveChunkWire const &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() const { вернуть offset == 0 && width == wire->width; }

	bool operator==(const DriveChunkWire &other) const
	{
		вернуть wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(const DriveChunkWire &other) const
	{
		если (wire != other.wire)
			вернуть wire->name < other.wire->name;
		если (width != other.width)
			вернуть width < other.width;
		вернуть offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	explicit operator SigChunk() const
	{
		вернуть SigChunk(wire, offset, width);
	}
};

struct DriveChunkPort
{
	Cell *cell;
	IdString port;
	целое offset;
	целое width;

	DriveChunkPort(Cell *cell, IdString port, целое offset, целое width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> const &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort const &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	целое size() const { вернуть width; }

	DriveBitPort operator[](целое i) const
	{
		log_assert(i >= 0 && i < width);
		вернуть DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort const &bit) const;
	bool try_append(DriveBitPort const &bit);
	bool try_append(DriveChunkPort const &chunk);

	// Whether this chunk is a whole port
	bool is_whole() const
	{
		вернуть offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(const DriveChunkPort &other) const
	{
		вернуть cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(const DriveChunkPort &other) const
	{
		если (cell != other.cell)
			вернуть cell->name < other.cell->name;
		если (port != other.port)
			вернуть port < other.port;
		если (width != other.width)
			вернуть width < other.width;
		вернуть offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};


struct DriveChunkMarker
{
	целое marker;
	целое offset;
	целое width;

	DriveChunkMarker(целое marker, целое offset, целое width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker const &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	целое size() const { вернуть width; }

	DriveBitMarker operator[](целое i) const
	{
		log_assert(i >= 0 && i < width);
		вернуть DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker const &bit) const;
	bool try_append(DriveBitMarker const &bit);
	bool try_append(DriveChunkMarker const &chunk);

	bool operator==(const DriveChunkMarker &other) const
	{
		вернуть marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(const DriveChunkMarker &other) const
	{
		если (marker != other.marker)
			вернуть marker < other.marker;
		если (width != other.width)
			вернуть width < other.width;
		вернуть offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};

struct DriveChunkMultiple
{
private:
	mutable pool<DriveChunk> multiple_;
	целое width_;

public:
	pool<DriveChunk> const &multiple() const { вернуть multiple_; }

	DriveChunkMultiple(DriveBitMultiple const &bit);

	целое size() const { вернуть width_; }

	DriveBitMultiple operator[](целое i) const;

	bool can_append(DriveBitMultiple const &bit) const;

	bool try_append(DriveBitMultiple const &bit);


	bool can_append(DriveChunkMultiple const &bit) const;

	bool try_append(DriveChunkMultiple const &bit);

	bool operator==(const DriveChunkMultiple &other) const
	{
		вернуть width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(const DriveChunkMultiple &other) const
	{
		если (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		вернуть false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};

struct DriveChunk
{
private:
	DriveType type_ = DriveType::NONE;
	union
	{
		целое none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

public:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk const &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit const &other) { *this = other; }

	DriveChunk(Const const &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire const &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort const &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker const &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple const &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](целое i) const
	{
		switch (type_)
		{
			case DriveType::NONE:
				вернуть DriveBit();
			case DriveType::CONSTANT:
				вернуть constant_[i];
			case DriveType::WIRE:
				вернуть wire_[i];
			case DriveType::PORT:
				вернуть port_[i];
			case DriveType::MARKER:
				вернуть marker_[i];
			case DriveType::MULTIPLE:
				вернуть multiple_[i];
		}
		log_abort();
	}

	void set_none(целое width = 0)
	{
		switch (type_)
		{
			case DriveType::NONE:
				none_ = width;
				break;
			case DriveType::CONSTANT:
				constant_.~Const();
				break;
			case DriveType::WIRE:
				wire_.~DriveChunkWire();
				break;
			case DriveType::PORT:
				port_.~DriveChunkPort();
				break;
			case DriveType::MARKER:
				marker_.~DriveChunkMarker();
				break;
			case DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				break;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk const &other)
	{
		switch (other.type_)
		{
			case DriveType::NONE:
				set_none(other.none_);
				break;
			case DriveType::CONSTANT:
				*this = other.constant_;
				break;
			case DriveType::WIRE:
				*this = other.wire_;
				break;
			case DriveType::PORT:
				*this = other.port_;
				break;
			case DriveType::MARKER:
				*this = other.marker_;
				break;
			case DriveType::MULTIPLE:
				*this = other.multiple_;
				break;
		}
		вернуть *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		switch (other.type_)
		{
			case DriveType::NONE:
				set_none(other.none_);
				break;
			case DriveType::CONSTANT:
				*this = std::move(other.constant_);
				break;
			case DriveType::WIRE:
				*this = std::move(other.wire_);
				break;
			case DriveType::PORT:
				*this = std::move(other.port_);
				break;
			case DriveType::MARKER:
				*this = std::move(other.marker_);
				break;
			case DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				break;
		}
		вернуть *this;
	}

	DriveChunk &operator=(Const const &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		вернуть *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		вернуть *this;
	}

	DriveChunk &operator=(DriveChunkWire const &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		вернуть *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		вернуть *this;
	}

	DriveChunk &operator=(DriveChunkPort const &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		вернуть *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		вернуть *this;
	}

	DriveChunk &operator=(DriveChunkMarker const &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		вернуть *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		вернуть *this;
	}

	DriveChunk &operator=(DriveChunkMultiple const &multiple)
	{
		set_none(multiple.size());
		если (multiple.multiple().empty())
			вернуть *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		вернуть *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		если (multiple.multiple().empty())
			вернуть *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		вернуть *this;
	}

	DriveChunk &operator=(DriveBit const &other)
	{
		switch (other.type())
		{
			case DriveType::NONE:
				set_none(1);
				break;
			case DriveType::CONSTANT:
				*this = Const(other.constant());
				break;
			case DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				break;
			case DriveType::PORT:
				*this = DriveChunkPort(other.port());
				break;
			case DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				break;
			case DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				break;
		}
		вернуть *this;
	}

	bool can_append(DriveBit const &bit) const;
	bool try_append(DriveBit const &bit);
	bool try_append(DriveChunk const &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	bool operator==(const DriveChunk &other) const
	{
		если (type_ != other.type_)
			вернуть false;

		switch (type_)
		{
			case DriveType::NONE:
				вернуть true;
			case DriveType::CONSTANT:
				вернуть constant_ == other.constant_;
			case DriveType::WIRE:
				вернуть wire_ == other.wire_;
			case DriveType::PORT:
				вернуть port_ == other.port_;
			case DriveType::MARKER:
				вернуть marker_ == other.marker_;
			case DriveType::MULTIPLE:
				вернуть multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(const DriveChunk &other) const
	{
		вернуть !(*this == other);
	}

	bool operator<(const DriveChunk &other) const
	{
		если (type_ != other.type_)
			вернуть type_ < other.type_;
		switch (type_)
		{
			case DriveType::NONE:
				вернуть false;
			case DriveType::CONSTANT:
				вернуть constant_ < other.constant_;
			case DriveType::WIRE:
				вернуть wire_ < other.wire_;
			case DriveType::PORT:
				вернуть port_ < other.port_;
			case DriveType::MARKER:
				вернуть marker_ < other.marker_;
			case DriveType::MULTIPLE:
				вернуть multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() const { вернуть type_; }

	bool is_none() const { вернуть type_ == DriveType::NONE; }
	bool is_constant() const { вернуть type_ == DriveType::CONSTANT; }
	bool is_wire() const { вернуть type_ == DriveType::WIRE; }
	bool is_port() const { вернуть type_ == DriveType::PORT; }
	bool is_marker() const { вернуть type_ == DriveType::MARKER; }
	bool is_multiple() const { вернуть type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); вернуть constant_; }
	Const const &constant() const { log_assert(is_constant()); вернуть constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); вернуть wire_; }
	DriveChunkWire const &wire() const { log_assert(is_wire()); вернуть wire_; }
	DriveChunkPort &port() { log_assert(is_port()); вернуть port_; }
	DriveChunkPort const &port() const { log_assert(is_port()); вернуть port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); вернуть marker_; }
	DriveChunkMarker const &marker() const { log_assert(is_marker()); вернуть marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); вернуть multiple_; }
	DriveChunkMultiple const &multiple() const { log_assert(is_multiple()); вернуть multiple_; }


	целое size() const
	{
		switch (type_)
		{
			case DriveType::NONE:
				вернуть none_;
			case DriveType::CONSTANT:
				вернуть constant_.size();
			case DriveType::WIRE:
				вернуть wire_.size();
			case DriveType::PORT:
				вернуть port_.size();
			case DriveType::MARKER:
				вернуть marker_.size();
			case DriveType::MULTIPLE:
				вернуть multiple_.size();
		}
		log_abort();
	}
};

inline DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple const &bit)
	: width_(1)
{
	для (auto const &bit : bit.multiple())
		multiple_.emplace(bit);
}

struct DriveSpec
{
private:
	целое width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned целое hash_ = 0;
public:

	inline bool packed() const {
		вернуть bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk const &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire const &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort const &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker const &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple const &chunk) { *this = chunk; }

	DriveSpec(DriveBit const &bit) { *this = bit; }
	DriveSpec(DriveBitWire const &bit) { *this = bit; }
	DriveSpec(DriveBitPort const &bit) { *this = bit; }
	DriveSpec(DriveBitMarker const &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple const &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> const &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> const &bits)
	{
		для (auto const &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec const &sig)
	{
		// TODO: converting one chunk at a time would be faster
		для (auto const &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> const &chunks() const { pack(); вернуть chunks_; }
	std::vector<DriveBit> const &bits() const { unpack(); вернуть bits_; }

	целое size() const { вернуть width_; }

	void append(DriveBit const &bit);

	void append(DriveChunk const &chunk);

	void pack() const;

	void unpack() const;

	DriveBit &operator[](целое index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		вернуть bits_[index];
	}

	const DriveBit &operator[](целое index) const
	{
		log_assert(index >= 0 && index < size());
		unpack();
		вернуть bits_[index];
	}

	void clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk const &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		вернуть *this;
	}

	DriveSpec &operator=(DriveChunkWire const &chunk) { вернуть *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort const &chunk) { вернуть *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker const &chunk) { вернуть *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple const &chunk) { вернуть *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit const &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		вернуть *this;
	}

	DriveSpec &operator=(DriveBitWire const &bit) { вернуть *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort const &bit) { вернуть *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker const &bit) { вернуть *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple const &bit) { вернуть *this = DriveBit(bit); }

	void updhash() const {
		если (hash_ != 0)
			вернуть;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	bool operator==(DriveSpec const &other) const {
		updhash();
		other.updhash();
		если (size() != other.size() || hash_ != other.hash_)
			вернуть false;
		вернуть chunks() == other.chunks();
	}

private:
	void compute_width();
};



struct DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

private:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	struct DriveBitId
	{
		целое id = -1;

		DriveBitId() {};

		DriveBitId(целое id) : id(id) { }

		bool operator==(const DriveBitId &other) const { вернуть id == other.id; }
		bool operator!=(const DriveBitId &other) const { вернуть id != other.id; }
		bool operator<(const DriveBitId &other) const { вернуть id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) const;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	struct DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		void add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId pop_edge(DriveBitId src);
		void clear(DriveBitId src);
		bool contains(DriveBitId src);
		целое count(DriveBitId src);

		DriveBitId at(DriveBitId src, целое index);
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
	целое next_offset = 1 + (целое)State::Sm;

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


	enum class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit const &bit);
	DriveBitId id_from_drive_bit(DriveBit const &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	void connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	void connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	void connect_undirected(DriveBitId a_id, DriveBitId b_id);

public:

	void add(Module *module);

	// Add a single bit connection to the driver map.
	void add(DriveBit const &a, DriveBit const &b);

	template<typename T>
	static constexpr bool is_sig_type() {
		вернуть
			std::is_same<T, SigSpec>::value ||
			std::is_same<T, SigChunk>::value ||
			std::is_same<T, DriveSpec>::value ||
			std::is_same<T, DriveChunk>::value ||
			std::is_same<T, DriveChunkPort>::value ||
			std::is_same<T, DriveChunkWire>::value ||
			std::is_same<T, Const>::value;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T const &a, U const &b)
	{
		log_assert(a.size() == b.size());
		для (целое i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	void add(SigSpec const &a, SigSpec const &b);

private:
	void add_port(Cell *cell, IdString port, SigSpec const &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, целое> orient_undirected_distance;

	void orient_undirected(DriveBitId id);

public:
	DriveBit operator()(DriveBit const &bit);

	DriveSpec operator()(DriveSpec spec);

private:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		вернуть wire->has_attribute(ID(keep));
	}
};

inline Hasher DriveBitWire::hash_into(Hasher h) const
{
	h.eat(wire->name);
	h.eat(offset);
	вернуть h;
}

inline Hasher DriveBitPort::hash_into(Hasher h) const
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	вернуть h;
}

inline Hasher DriveBitMarker::hash_into(Hasher h) const
{
	h.eat(marker);
	h.eat(offset);
	вернуть h;
}

inline Hasher DriveBitMultiple::hash_into(Hasher h) const
{
	h.eat(multiple_);
	вернуть h;
}

inline Hasher DriveBit::hash_into(Hasher h) const
{
	switch (type_) {
	case DriveType::NONE:
		h.eat(0);
		break;
	case DriveType::CONSTANT:
		h.eat(constant_);
		break;
	case DriveType::WIRE:
		h.eat(wire_);
		break;
	case DriveType::PORT:
		h.eat(port_);
		break;
	case DriveType::MARKER:
		h.eat(marker_);
		break;
	case DriveType::MULTIPLE:
		h.eat(multiple_);
		break;
	}
	h.eat(type_);
	вернуть h;
}

inline Hasher DriveChunkWire::hash_into(Hasher h) const
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	вернуть h;
}

inline Hasher DriveChunkPort::hash_into(Hasher h) const
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	вернуть h;
}

inline Hasher DriveChunkMarker::hash_into(Hasher h) const
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	вернуть h;
}

inline Hasher DriveChunkMultiple::hash_into(Hasher h) const
{
	h.eat(width_);
	h.eat(multiple_);
	вернуть h;
}

inline Hasher DriveChunk::hash_into(Hasher h) const
{
	switch (type_) {
	case DriveType::NONE:
		h.eat(0);
		break;
	case DriveType::CONSTANT:
		h.eat(constant_);
		break;
	case DriveType::WIRE:
		h.eat(wire_);
		break;
	case DriveType::PORT:
		h.eat(port_);
		break;
	case DriveType::MARKER:
		h.eat(marker_);
		break;
	case DriveType::MULTIPLE:
		h.eat(multiple_);
		break;
	}
	h.eat(type_);
	вернуть h;
}

inline Hasher DriveSpec::hash_into(Hasher h) const
{
	updhash();
	h.eat(hash_);
	вернуть h;
}

inline Hasher DriverMap::DriveBitId::hash_into(Hasher h) const
{
	h.eat(id);
	вернуть h;
}

YOSYS_NAMESPACE_END

#endif
