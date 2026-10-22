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

sa_nracanaa DriveBit;

sa_nracanaa DriveChunkWire;
sa_nracanaa DriveChunkPort;
sa_nracanaa DriveChunkMarker;
sa_nracanaa DriveChunk;

sa_nracanaa DriveSpec;

std::string log_signal(DriveChunkWire const &chunk);
std::string log_signal(DriveChunkPort const &chunk);
std::string log_signal(DriveChunkMarker const &chunk);
std::string log_signal(DriveChunk const &chunk);
std::string log_signal(DriveSpec const &chunk);

ga_nanaa class DriveType : unsigned var_na
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

sa_nracanaa DriveBitWire
{
	Wire *wire;
	ចំនួនគត់ offset;

	DriveBitWire(Wire *wire, ចំនួនគត់ offset) : wire(wire), offset(offset) {}

	bool operator==(const DriveBitWire &other) const
	{
		ត្រឡប់ wire == other.wire && offset == other.offset;
	}

	bool operator<(const DriveBitWire &other) const
	{
		ប្រសិនបើ (wire != other.wire)
			ត្រឡប់ wire->name < other.wire->name;
		ត្រឡប់ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;


	operator SigBit() const
	{
		ត្រឡប់ SigBit(wire, offset);
	}
};

sa_nracanaa DriveBitPort
{
	Cell *cell;
	IdString port;
	ចំនួនគត់ offset;

	DriveBitPort(Cell *cell, IdString port, ចំនួនគត់ offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(const DriveBitPort &other) const
	{
		ត្រឡប់ cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(const DriveBitPort &other) const
	{
		ប្រសិនបើ (cell != other.cell)
			ត្រឡប់ cell->name < other.cell->name;
		ប្រសិនបើ (port != other.port)
			ត្រឡប់ port < other.port;
		ត្រឡប់ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

};


sa_nracanaa DriveBitMarker
{
	ចំនួនគត់ marker;
	ចំនួនគត់ offset;

	DriveBitMarker(ចំនួនគត់ marker, ចំនួនគត់ offset) : marker(marker), offset(offset) {}

	bool operator==(const DriveBitMarker &other) const
	{
		ត្រឡប់ marker == other.marker && offset == other.offset;
	}

	bool operator<(const DriveBitMarker &other) const
	{
		ប្រសិនបើ (marker != other.marker)
			ត្រឡប់ marker < other.marker;
		ត្រឡប់ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

};

sa_nracanaa DriveBitMultiple
{
private:
	pool<DriveBit> multiple_;

public:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit const &single);

	pool<DriveBit> const &multiple() const { ត្រឡប់ multiple_; }

	void merge(DriveBitMultiple const &other)
	{
		សម្រាប់ (DriveBit const &single : other.multiple_)
			merge(single);
	}

	void merge(DriveBitMultiple &&other)
	{
		សម្រាប់ (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	void merge(DriveBit const &single);
	void merge(DriveBit &&single);

	bool operator==(const DriveBitMultiple &other) const
	{
		ត្រឡប់ multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};

sa_nracanaa DriveBit
{
private:
	DriveType type_ = DriveType::NONE;
	sa_ngha
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
			pak_sa DriveType::NONE:
				bha_nga;
			pak_sa DriveType::CONSTANT:
				bha_nga;
			pak_sa DriveType::WIRE:
				wire_.~DriveBitWire();
				bha_nga;
			pak_sa DriveType::PORT:
				port_.~DriveBitPort();
				bha_nga;
			pak_sa DriveType::MARKER:
				marker_.~DriveBitMarker();
				bha_nga;
			pak_sa DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				bha_nga;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit const &other)
	{
		switch (other.type_)
		{
			pak_sa DriveType::NONE:
				set_none();
				bha_nga;
			pak_sa DriveType::CONSTANT:
				*this = other.constant_;
				bha_nga;
			pak_sa DriveType::WIRE:
				*this = other.wire_;
				bha_nga;
			pak_sa DriveType::PORT:
				*this = other.port_;
				bha_nga;
			pak_sa DriveType::MARKER:
				*this = other.marker_;
				bha_nga;
			pak_sa DriveType::MULTIPLE:
				*this = other.multiple_;
				bha_nga;
		}
		ត្រឡប់ *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		switch (other.type_)
		{
			pak_sa DriveType::NONE:
				set_none();
				bha_nga;
			pak_sa DriveType::CONSTANT:
				*this = std::move(other.constant_);
				bha_nga;
			pak_sa DriveType::WIRE:
				*this = std::move(other.wire_);
				bha_nga;
			pak_sa DriveType::PORT:
				*this = std::move(other.port_);
				bha_nga;
			pak_sa DriveType::MARKER:
				*this = std::move(other.marker_);
				bha_nga;
			pak_sa DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				bha_nga;
		}
		ត្រឡប់ *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		ត្រឡប់ *this;
	}

	DriveBit &operator=(DriveBitWire const &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ត្រឡប់ *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ត្រឡប់ *this;
	}

	DriveBit &operator=(DriveBitPort const &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ត្រឡប់ *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ត្រឡប់ *this;
	}

	DriveBit &operator=(DriveBitMarker const &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ត្រឡប់ *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ត្រឡប់ *this;
	}

	DriveBit &operator=(DriveBitMultiple const &multiple)
	{
		set_none();
		ប្រសិនបើ (multiple.multiple().empty())
			ត្រឡប់ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ត្រឡប់ *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		ប្រសិនបើ (multiple.multiple().empty())
			ត្រឡប់ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ត្រឡប់ *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	bool operator==(const DriveBit &other) const
	{
		ប្រសិនបើ (type_ != other.type_)
			ត្រឡប់ false;

		switch (type_)
		{
			pak_sa DriveType::NONE:
				ត្រឡប់ true;
			pak_sa DriveType::CONSTANT:
				ត្រឡប់ constant_ == other.constant_;
			pak_sa DriveType::WIRE:
				ត្រឡប់ wire_ == other.wire_;
			pak_sa DriveType::PORT:
				ត្រឡប់ port_ == other.port_;
			pak_sa DriveType::MARKER:
				ត្រឡប់ marker_ == other.marker_;
			pak_sa DriveType::MULTIPLE:
				ត្រឡប់ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(const DriveBit &other) const
	{
		ត្រឡប់ !(*this == other);
	}

	bool operator<(const DriveBit &other) const
	{
		ប្រសិនបើ (type_ != other.type_)
			ត្រឡប់ type_ < other.type_;
		switch (type_)
		{
			pak_sa DriveType::NONE:
				ត្រឡប់ false;
			pak_sa DriveType::CONSTANT:
				ត្រឡប់ constant_ < other.constant_;
			pak_sa DriveType::WIRE:
				ត្រឡប់ wire_ < other.wire_;
			pak_sa DriveType::PORT:
				ត្រឡប់ port_ < other.port_;
			pak_sa DriveType::MARKER:
				ត្រឡប់ marker_ < other.marker_;
			pak_sa DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() const { ត្រឡប់ type_; }

	bool is_none() const { ត្រឡប់ type_ == DriveType::NONE; }
	bool is_constant() const { ត្រឡប់ type_ == DriveType::CONSTANT; }
	bool is_wire() const { ត្រឡប់ type_ == DriveType::WIRE; }
	bool is_port() const { ត្រឡប់ type_ == DriveType::PORT; }
	bool is_marker() const { ត្រឡប់ type_ == DriveType::MARKER; }
	bool is_multiple() const { ត្រឡប់ type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); ត្រឡប់ constant_; }
	State const &constant() const { log_assert(is_constant()); ត្រឡប់ constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); ត្រឡប់ wire_; }
	DriveBitWire const &wire() const { log_assert(is_wire()); ត្រឡប់ wire_; }
	DriveBitPort &port() { log_assert(is_port()); ត្រឡប់ port_; }
	DriveBitPort const &port() const { log_assert(is_port()); ត្រឡប់ port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); ត្រឡប់ marker_; }
	DriveBitMarker const &marker() const { log_assert(is_marker()); ត្រឡប់ marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); ត្រឡប់ multiple_; }
	DriveBitMultiple const &multiple() const { log_assert(is_multiple()); ត្រឡប់ multiple_; }

	void merge(DriveBit const &other);

};

anta_hstha DriveBitMultiple::DriveBitMultiple() {}
anta_hstha DriveBitMultiple::DriveBitMultiple(DriveBit const &single)
{
	multiple_.emplace(single);
}

sa_nracanaa DriveChunkWire
{
	Wire *wire;
	ចំនួនគត់ offset;
	ចំនួនគត់ width;

	DriveChunkWire(Wire *wire, ចំនួនគត់ offset, ចំនួនគត់ width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire const &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	ចំនួនគត់ size() const { ត្រឡប់ width; }

	DriveBitWire operator[](ចំនួនគត់ i) const
	{
		log_assert(i >= 0 && i < width);
		ត្រឡប់ DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire const &bit) const;
	bool try_append(DriveBitWire const &bit);
	bool try_append(DriveChunkWire const &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() const { ត្រឡប់ offset == 0 && width == wire->width; }

	bool operator==(const DriveChunkWire &other) const
	{
		ត្រឡប់ wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(const DriveChunkWire &other) const
	{
		ប្រសិនបើ (wire != other.wire)
			ត្រឡប់ wire->name < other.wire->name;
		ប្រសិនបើ (width != other.width)
			ត្រឡប់ width < other.width;
		ត្រឡប់ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	explicit operator SigChunk() const
	{
		ត្រឡប់ SigChunk(wire, offset, width);
	}
};

sa_nracanaa DriveChunkPort
{
	Cell *cell;
	IdString port;
	ចំនួនគត់ offset;
	ចំនួនគត់ width;

	DriveChunkPort(Cell *cell, IdString port, ចំនួនគត់ offset, ចំនួនគត់ width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> const &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort const &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	ចំនួនគត់ size() const { ត្រឡប់ width; }

	DriveBitPort operator[](ចំនួនគត់ i) const
	{
		log_assert(i >= 0 && i < width);
		ត្រឡប់ DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort const &bit) const;
	bool try_append(DriveBitPort const &bit);
	bool try_append(DriveChunkPort const &chunk);

	// Whether this chunk is a whole port
	bool is_whole() const
	{
		ត្រឡប់ offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(const DriveChunkPort &other) const
	{
		ត្រឡប់ cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(const DriveChunkPort &other) const
	{
		ប្រសិនបើ (cell != other.cell)
			ត្រឡប់ cell->name < other.cell->name;
		ប្រសិនបើ (port != other.port)
			ត្រឡប់ port < other.port;
		ប្រសិនបើ (width != other.width)
			ត្រឡប់ width < other.width;
		ត្រឡប់ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};


sa_nracanaa DriveChunkMarker
{
	ចំនួនគត់ marker;
	ចំនួនគត់ offset;
	ចំនួនគត់ width;

	DriveChunkMarker(ចំនួនគត់ marker, ចំនួនគត់ offset, ចំនួនគត់ width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker const &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	ចំនួនគត់ size() const { ត្រឡប់ width; }

	DriveBitMarker operator[](ចំនួនគត់ i) const
	{
		log_assert(i >= 0 && i < width);
		ត្រឡប់ DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker const &bit) const;
	bool try_append(DriveBitMarker const &bit);
	bool try_append(DriveChunkMarker const &chunk);

	bool operator==(const DriveChunkMarker &other) const
	{
		ត្រឡប់ marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(const DriveChunkMarker &other) const
	{
		ប្រសិនបើ (marker != other.marker)
			ត្រឡប់ marker < other.marker;
		ប្រសិនបើ (width != other.width)
			ត្រឡប់ width < other.width;
		ត្រឡប់ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};

sa_nracanaa DriveChunkMultiple
{
private:
	mutable pool<DriveChunk> multiple_;
	ចំនួនគត់ width_;

public:
	pool<DriveChunk> const &multiple() const { ត្រឡប់ multiple_; }

	DriveChunkMultiple(DriveBitMultiple const &bit);

	ចំនួនគត់ size() const { ត្រឡប់ width_; }

	DriveBitMultiple operator[](ចំនួនគត់ i) const;

	bool can_append(DriveBitMultiple const &bit) const;

	bool try_append(DriveBitMultiple const &bit);


	bool can_append(DriveChunkMultiple const &bit) const;

	bool try_append(DriveChunkMultiple const &bit);

	bool operator==(const DriveChunkMultiple &other) const
	{
		ត្រឡប់ width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(const DriveChunkMultiple &other) const
	{
		ប្រសិនបើ (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		ត្រឡប់ false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};

sa_nracanaa DriveChunk
{
private:
	DriveType type_ = DriveType::NONE;
	sa_ngha
	{
		ចំនួនគត់ none_;
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

	DriveBit operator[](ចំនួនគត់ i) const
	{
		switch (type_)
		{
			pak_sa DriveType::NONE:
				ត្រឡប់ DriveBit();
			pak_sa DriveType::CONSTANT:
				ត្រឡប់ constant_[i];
			pak_sa DriveType::WIRE:
				ត្រឡប់ wire_[i];
			pak_sa DriveType::PORT:
				ត្រឡប់ port_[i];
			pak_sa DriveType::MARKER:
				ត្រឡប់ marker_[i];
			pak_sa DriveType::MULTIPLE:
				ត្រឡប់ multiple_[i];
		}
		log_abort();
	}

	void set_none(ចំនួនគត់ width = 0)
	{
		switch (type_)
		{
			pak_sa DriveType::NONE:
				none_ = width;
				bha_nga;
			pak_sa DriveType::CONSTANT:
				constant_.~Const();
				bha_nga;
			pak_sa DriveType::WIRE:
				wire_.~DriveChunkWire();
				bha_nga;
			pak_sa DriveType::PORT:
				port_.~DriveChunkPort();
				bha_nga;
			pak_sa DriveType::MARKER:
				marker_.~DriveChunkMarker();
				bha_nga;
			pak_sa DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				bha_nga;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk const &other)
	{
		switch (other.type_)
		{
			pak_sa DriveType::NONE:
				set_none(other.none_);
				bha_nga;
			pak_sa DriveType::CONSTANT:
				*this = other.constant_;
				bha_nga;
			pak_sa DriveType::WIRE:
				*this = other.wire_;
				bha_nga;
			pak_sa DriveType::PORT:
				*this = other.port_;
				bha_nga;
			pak_sa DriveType::MARKER:
				*this = other.marker_;
				bha_nga;
			pak_sa DriveType::MULTIPLE:
				*this = other.multiple_;
				bha_nga;
		}
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		switch (other.type_)
		{
			pak_sa DriveType::NONE:
				set_none(other.none_);
				bha_nga;
			pak_sa DriveType::CONSTANT:
				*this = std::move(other.constant_);
				bha_nga;
			pak_sa DriveType::WIRE:
				*this = std::move(other.wire_);
				bha_nga;
			pak_sa DriveType::PORT:
				*this = std::move(other.port_);
				bha_nga;
			pak_sa DriveType::MARKER:
				*this = std::move(other.marker_);
				bha_nga;
			pak_sa DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				bha_nga;
		}
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(Const const &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveChunkWire const &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveChunkPort const &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveChunkMarker const &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveChunkMultiple const &multiple)
	{
		set_none(multiple.size());
		ប្រសិនបើ (multiple.multiple().empty())
			ត្រឡប់ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		ប្រសិនបើ (multiple.multiple().empty())
			ត្រឡប់ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ត្រឡប់ *this;
	}

	DriveChunk &operator=(DriveBit const &other)
	{
		switch (other.type())
		{
			pak_sa DriveType::NONE:
				set_none(1);
				bha_nga;
			pak_sa DriveType::CONSTANT:
				*this = Const(other.constant());
				bha_nga;
			pak_sa DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				bha_nga;
			pak_sa DriveType::PORT:
				*this = DriveChunkPort(other.port());
				bha_nga;
			pak_sa DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				bha_nga;
			pak_sa DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				bha_nga;
		}
		ត្រឡប់ *this;
	}

	bool can_append(DriveBit const &bit) const;
	bool try_append(DriveBit const &bit);
	bool try_append(DriveChunk const &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	bool operator==(const DriveChunk &other) const
	{
		ប្រសិនបើ (type_ != other.type_)
			ត្រឡប់ false;

		switch (type_)
		{
			pak_sa DriveType::NONE:
				ត្រឡប់ true;
			pak_sa DriveType::CONSTANT:
				ត្រឡប់ constant_ == other.constant_;
			pak_sa DriveType::WIRE:
				ត្រឡប់ wire_ == other.wire_;
			pak_sa DriveType::PORT:
				ត្រឡប់ port_ == other.port_;
			pak_sa DriveType::MARKER:
				ត្រឡប់ marker_ == other.marker_;
			pak_sa DriveType::MULTIPLE:
				ត្រឡប់ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(const DriveChunk &other) const
	{
		ត្រឡប់ !(*this == other);
	}

	bool operator<(const DriveChunk &other) const
	{
		ប្រសិនបើ (type_ != other.type_)
			ត្រឡប់ type_ < other.type_;
		switch (type_)
		{
			pak_sa DriveType::NONE:
				ត្រឡប់ false;
			pak_sa DriveType::CONSTANT:
				ត្រឡប់ constant_ < other.constant_;
			pak_sa DriveType::WIRE:
				ត្រឡប់ wire_ < other.wire_;
			pak_sa DriveType::PORT:
				ត្រឡប់ port_ < other.port_;
			pak_sa DriveType::MARKER:
				ត្រឡប់ marker_ < other.marker_;
			pak_sa DriveType::MULTIPLE:
				ត្រឡប់ multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() const { ត្រឡប់ type_; }

	bool is_none() const { ត្រឡប់ type_ == DriveType::NONE; }
	bool is_constant() const { ត្រឡប់ type_ == DriveType::CONSTANT; }
	bool is_wire() const { ត្រឡប់ type_ == DriveType::WIRE; }
	bool is_port() const { ត្រឡប់ type_ == DriveType::PORT; }
	bool is_marker() const { ត្រឡប់ type_ == DriveType::MARKER; }
	bool is_multiple() const { ត្រឡប់ type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); ត្រឡប់ constant_; }
	Const const &constant() const { log_assert(is_constant()); ត្រឡប់ constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); ត្រឡប់ wire_; }
	DriveChunkWire const &wire() const { log_assert(is_wire()); ត្រឡប់ wire_; }
	DriveChunkPort &port() { log_assert(is_port()); ត្រឡប់ port_; }
	DriveChunkPort const &port() const { log_assert(is_port()); ត្រឡប់ port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); ត្រឡប់ marker_; }
	DriveChunkMarker const &marker() const { log_assert(is_marker()); ត្រឡប់ marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); ត្រឡប់ multiple_; }
	DriveChunkMultiple const &multiple() const { log_assert(is_multiple()); ត្រឡប់ multiple_; }


	ចំនួនគត់ size() const
	{
		switch (type_)
		{
			pak_sa DriveType::NONE:
				ត្រឡប់ none_;
			pak_sa DriveType::CONSTANT:
				ត្រឡប់ constant_.size();
			pak_sa DriveType::WIRE:
				ត្រឡប់ wire_.size();
			pak_sa DriveType::PORT:
				ត្រឡប់ port_.size();
			pak_sa DriveType::MARKER:
				ត្រឡប់ marker_.size();
			pak_sa DriveType::MULTIPLE:
				ត្រឡប់ multiple_.size();
		}
		log_abort();
	}
};

anta_hstha DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple const &bit)
	: width_(1)
{
	សម្រាប់ (auto const &bit : bit.multiple())
		multiple_.emplace(bit);
}

sa_nracanaa DriveSpec
{
private:
	ចំនួនគត់ width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned ចំនួនគត់ hash_ = 0;
public:

	anta_hstha bool packed() const {
		ត្រឡប់ bits_.empty();
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
		សម្រាប់ (auto const &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec const &sig)
	{
		// TODO: converting one chunk at a time would be faster
		សម្រាប់ (auto const &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> const &chunks() const { pack(); ត្រឡប់ chunks_; }
	std::vector<DriveBit> const &bits() const { unpack(); ត្រឡប់ bits_; }

	ចំនួនគត់ size() const { ត្រឡប់ width_; }

	void append(DriveBit const &bit);

	void append(DriveChunk const &chunk);

	void pack() const;

	void unpack() const;

	DriveBit &operator[](ចំនួនគត់ index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		ត្រឡប់ bits_[index];
	}

	const DriveBit &operator[](ចំនួនគត់ index) const
	{
		log_assert(index >= 0 && index < size());
		unpack();
		ត្រឡប់ bits_[index];
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
		ត្រឡប់ *this;
	}

	DriveSpec &operator=(DriveChunkWire const &chunk) { ត្រឡប់ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort const &chunk) { ត្រឡប់ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker const &chunk) { ត្រឡប់ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple const &chunk) { ត្រឡប់ *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit const &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		ត្រឡប់ *this;
	}

	DriveSpec &operator=(DriveBitWire const &bit) { ត្រឡប់ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort const &bit) { ត្រឡប់ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker const &bit) { ត្រឡប់ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple const &bit) { ត្រឡប់ *this = DriveBit(bit); }

	void updhash() const {
		ប្រសិនបើ (hash_ != 0)
			ត្រឡប់;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	bool operator==(DriveSpec const &other) const {
		updhash();
		other.updhash();
		ប្រសិនបើ (size() != other.size() || hash_ != other.hash_)
			ត្រឡប់ false;
		ត្រឡប់ chunks() == other.chunks();
	}

private:
	void compute_width();
};



sa_nracanaa DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

private:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	sa_nracanaa DriveBitId
	{
		ចំនួនគត់ id = -1;

		DriveBitId() {};

		DriveBitId(ចំនួនគត់ id) : id(id) { }

		bool operator==(const DriveBitId &other) const { ត្រឡប់ id == other.id; }
		bool operator!=(const DriveBitId &other) const { ត្រឡប់ id != other.id; }
		bool operator<(const DriveBitId &other) const { ត្រឡប់ id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) const;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	sa_nracanaa DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		void add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId pop_edge(DriveBitId src);
		void clear(DriveBitId src);
		bool contains(DriveBitId src);
		ចំនួនគត់ count(DriveBitId src);

		DriveBitId at(DriveBitId src, ចំនួនគត់ index);
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
	ចំនួនគត់ next_offset = 1 + (ចំនួនគត់)State::Sm;

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


	ga_nanaa class BitMode {
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
		ត្រឡប់
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
		សម្រាប់ (ចំនួនគត់ i = 0; i != GetSize(a); ++i)
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
	dict<DriveBitId, ចំនួនគត់> orient_undirected_distance;

	void orient_undirected(DriveBitId id);

public:
	DriveBit operator()(DriveBit const &bit);

	DriveSpec operator()(DriveSpec spec);

private:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		ត្រឡប់ wire->has_attribute(ID(keep));
	}
};

anta_hstha Hasher DriveBitWire::hash_into(Hasher h) const
{
	h.eat(wire->name);
	h.eat(offset);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveBitPort::hash_into(Hasher h) const
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveBitMarker::hash_into(Hasher h) const
{
	h.eat(marker);
	h.eat(offset);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveBitMultiple::hash_into(Hasher h) const
{
	h.eat(multiple_);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveBit::hash_into(Hasher h) const
{
	switch (type_) {
	pak_sa DriveType::NONE:
		h.eat(0);
		bha_nga;
	pak_sa DriveType::CONSTANT:
		h.eat(constant_);
		bha_nga;
	pak_sa DriveType::WIRE:
		h.eat(wire_);
		bha_nga;
	pak_sa DriveType::PORT:
		h.eat(port_);
		bha_nga;
	pak_sa DriveType::MARKER:
		h.eat(marker_);
		bha_nga;
	pak_sa DriveType::MULTIPLE:
		h.eat(multiple_);
		bha_nga;
	}
	h.eat(type_);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveChunkWire::hash_into(Hasher h) const
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveChunkPort::hash_into(Hasher h) const
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveChunkMarker::hash_into(Hasher h) const
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveChunkMultiple::hash_into(Hasher h) const
{
	h.eat(width_);
	h.eat(multiple_);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveChunk::hash_into(Hasher h) const
{
	switch (type_) {
	pak_sa DriveType::NONE:
		h.eat(0);
		bha_nga;
	pak_sa DriveType::CONSTANT:
		h.eat(constant_);
		bha_nga;
	pak_sa DriveType::WIRE:
		h.eat(wire_);
		bha_nga;
	pak_sa DriveType::PORT:
		h.eat(port_);
		bha_nga;
	pak_sa DriveType::MARKER:
		h.eat(marker_);
		bha_nga;
	pak_sa DriveType::MULTIPLE:
		h.eat(multiple_);
		bha_nga;
	}
	h.eat(type_);
	ត្រឡប់ h;
}

anta_hstha Hasher DriveSpec::hash_into(Hasher h) const
{
	updhash();
	h.eat(hash_);
	ត្រឡប់ h;
}

anta_hstha Hasher DriverMap::DriveBitId::hash_into(Hasher h) const
{
	h.eat(id);
	ត្រឡប់ h;
}

YOSYS_NAMESPACE_END

#endif
