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
	ကိန်းပြည့် offset;

	DriveBitWire(Wire *wire, ကိန်းပြည့် offset) : wire(wire), offset(offset) {}

	bool operator==(const DriveBitWire &other) const
	{
		ပြန်ပေး wire == other.wire && offset == other.offset;
	}

	bool operator<(const DriveBitWire &other) const
	{
		အကယ်၍ (wire != other.wire)
			ပြန်ပေး wire->name < other.wire->name;
		ပြန်ပေး offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;


	operator SigBit() const
	{
		ပြန်ပေး SigBit(wire, offset);
	}
};

sa_nracanaa DriveBitPort
{
	Cell *cell;
	IdString port;
	ကိန်းပြည့် offset;

	DriveBitPort(Cell *cell, IdString port, ကိန်းပြည့် offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(const DriveBitPort &other) const
	{
		ပြန်ပေး cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(const DriveBitPort &other) const
	{
		အကယ်၍ (cell != other.cell)
			ပြန်ပေး cell->name < other.cell->name;
		အကယ်၍ (port != other.port)
			ပြန်ပေး port < other.port;
		ပြန်ပေး offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

};


sa_nracanaa DriveBitMarker
{
	ကိန်းပြည့် marker;
	ကိန်းပြည့် offset;

	DriveBitMarker(ကိန်းပြည့် marker, ကိန်းပြည့် offset) : marker(marker), offset(offset) {}

	bool operator==(const DriveBitMarker &other) const
	{
		ပြန်ပေး marker == other.marker && offset == other.offset;
	}

	bool operator<(const DriveBitMarker &other) const
	{
		အကယ်၍ (marker != other.marker)
			ပြန်ပေး marker < other.marker;
		ပြန်ပေး offset < other.offset;
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

	pool<DriveBit> const &multiple() const { ပြန်ပေး multiple_; }

	void merge(DriveBitMultiple const &other)
	{
		အတွက် (DriveBit const &single : other.multiple_)
			merge(single);
	}

	void merge(DriveBitMultiple &&other)
	{
		အတွက် (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	void merge(DriveBit const &single);
	void merge(DriveBit &&single);

	bool operator==(const DriveBitMultiple &other) const
	{
		ပြန်ပေး multiple_ == other.multiple_;
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
		ပြန်ပေး *this;
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
		ပြန်ပေး *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		ပြန်ပေး *this;
	}

	DriveBit &operator=(DriveBitWire const &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ပြန်ပေး *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ပြန်ပေး *this;
	}

	DriveBit &operator=(DriveBitPort const &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ပြန်ပေး *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ပြန်ပေး *this;
	}

	DriveBit &operator=(DriveBitMarker const &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ပြန်ပေး *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ပြန်ပေး *this;
	}

	DriveBit &operator=(DriveBitMultiple const &multiple)
	{
		set_none();
		အကယ်၍ (multiple.multiple().empty())
			ပြန်ပေး *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ပြန်ပေး *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		အကယ်၍ (multiple.multiple().empty())
			ပြန်ပေး *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ပြန်ပေး *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	bool operator==(const DriveBit &other) const
	{
		အကယ်၍ (type_ != other.type_)
			ပြန်ပေး false;

		switch (type_)
		{
			pak_sa DriveType::NONE:
				ပြန်ပေး true;
			pak_sa DriveType::CONSTANT:
				ပြန်ပေး constant_ == other.constant_;
			pak_sa DriveType::WIRE:
				ပြန်ပေး wire_ == other.wire_;
			pak_sa DriveType::PORT:
				ပြန်ပေး port_ == other.port_;
			pak_sa DriveType::MARKER:
				ပြန်ပေး marker_ == other.marker_;
			pak_sa DriveType::MULTIPLE:
				ပြန်ပေး multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(const DriveBit &other) const
	{
		ပြန်ပေး !(*this == other);
	}

	bool operator<(const DriveBit &other) const
	{
		အကယ်၍ (type_ != other.type_)
			ပြန်ပေး type_ < other.type_;
		switch (type_)
		{
			pak_sa DriveType::NONE:
				ပြန်ပေး false;
			pak_sa DriveType::CONSTANT:
				ပြန်ပေး constant_ < other.constant_;
			pak_sa DriveType::WIRE:
				ပြန်ပေး wire_ < other.wire_;
			pak_sa DriveType::PORT:
				ပြန်ပေး port_ < other.port_;
			pak_sa DriveType::MARKER:
				ပြန်ပေး marker_ < other.marker_;
			pak_sa DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() const { ပြန်ပေး type_; }

	bool is_none() const { ပြန်ပေး type_ == DriveType::NONE; }
	bool is_constant() const { ပြန်ပေး type_ == DriveType::CONSTANT; }
	bool is_wire() const { ပြန်ပေး type_ == DriveType::WIRE; }
	bool is_port() const { ပြန်ပေး type_ == DriveType::PORT; }
	bool is_marker() const { ပြန်ပေး type_ == DriveType::MARKER; }
	bool is_multiple() const { ပြန်ပေး type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); ပြန်ပေး constant_; }
	State const &constant() const { log_assert(is_constant()); ပြန်ပေး constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); ပြန်ပေး wire_; }
	DriveBitWire const &wire() const { log_assert(is_wire()); ပြန်ပေး wire_; }
	DriveBitPort &port() { log_assert(is_port()); ပြန်ပေး port_; }
	DriveBitPort const &port() const { log_assert(is_port()); ပြန်ပေး port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); ပြန်ပေး marker_; }
	DriveBitMarker const &marker() const { log_assert(is_marker()); ပြန်ပေး marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); ပြန်ပေး multiple_; }
	DriveBitMultiple const &multiple() const { log_assert(is_multiple()); ပြန်ပေး multiple_; }

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
	ကိန်းပြည့် offset;
	ကိန်းပြည့် width;

	DriveChunkWire(Wire *wire, ကိန်းပြည့် offset, ကိန်းပြည့် width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire const &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	ကိန်းပြည့် size() const { ပြန်ပေး width; }

	DriveBitWire operator[](ကိန်းပြည့် i) const
	{
		log_assert(i >= 0 && i < width);
		ပြန်ပေး DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire const &bit) const;
	bool try_append(DriveBitWire const &bit);
	bool try_append(DriveChunkWire const &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() const { ပြန်ပေး offset == 0 && width == wire->width; }

	bool operator==(const DriveChunkWire &other) const
	{
		ပြန်ပေး wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(const DriveChunkWire &other) const
	{
		အကယ်၍ (wire != other.wire)
			ပြန်ပေး wire->name < other.wire->name;
		အကယ်၍ (width != other.width)
			ပြန်ပေး width < other.width;
		ပြန်ပေး offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	explicit operator SigChunk() const
	{
		ပြန်ပေး SigChunk(wire, offset, width);
	}
};

sa_nracanaa DriveChunkPort
{
	Cell *cell;
	IdString port;
	ကိန်းပြည့် offset;
	ကိန်းပြည့် width;

	DriveChunkPort(Cell *cell, IdString port, ကိန်းပြည့် offset, ကိန်းပြည့် width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> const &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort const &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	ကိန်းပြည့် size() const { ပြန်ပေး width; }

	DriveBitPort operator[](ကိန်းပြည့် i) const
	{
		log_assert(i >= 0 && i < width);
		ပြန်ပေး DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort const &bit) const;
	bool try_append(DriveBitPort const &bit);
	bool try_append(DriveChunkPort const &chunk);

	// Whether this chunk is a whole port
	bool is_whole() const
	{
		ပြန်ပေး offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(const DriveChunkPort &other) const
	{
		ပြန်ပေး cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(const DriveChunkPort &other) const
	{
		အကယ်၍ (cell != other.cell)
			ပြန်ပေး cell->name < other.cell->name;
		အကယ်၍ (port != other.port)
			ပြန်ပေး port < other.port;
		အကယ်၍ (width != other.width)
			ပြန်ပေး width < other.width;
		ပြန်ပေး offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};


sa_nracanaa DriveChunkMarker
{
	ကိန်းပြည့် marker;
	ကိန်းပြည့် offset;
	ကိန်းပြည့် width;

	DriveChunkMarker(ကိန်းပြည့် marker, ကိန်းပြည့် offset, ကိန်းပြည့် width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker const &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	ကိန်းပြည့် size() const { ပြန်ပေး width; }

	DriveBitMarker operator[](ကိန်းပြည့် i) const
	{
		log_assert(i >= 0 && i < width);
		ပြန်ပေး DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker const &bit) const;
	bool try_append(DriveBitMarker const &bit);
	bool try_append(DriveChunkMarker const &chunk);

	bool operator==(const DriveChunkMarker &other) const
	{
		ပြန်ပေး marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(const DriveChunkMarker &other) const
	{
		အကယ်၍ (marker != other.marker)
			ပြန်ပေး marker < other.marker;
		အကယ်၍ (width != other.width)
			ပြန်ပေး width < other.width;
		ပြန်ပေး offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};

sa_nracanaa DriveChunkMultiple
{
private:
	mutable pool<DriveChunk> multiple_;
	ကိန်းပြည့် width_;

public:
	pool<DriveChunk> const &multiple() const { ပြန်ပေး multiple_; }

	DriveChunkMultiple(DriveBitMultiple const &bit);

	ကိန်းပြည့် size() const { ပြန်ပေး width_; }

	DriveBitMultiple operator[](ကိန်းပြည့် i) const;

	bool can_append(DriveBitMultiple const &bit) const;

	bool try_append(DriveBitMultiple const &bit);


	bool can_append(DriveChunkMultiple const &bit) const;

	bool try_append(DriveChunkMultiple const &bit);

	bool operator==(const DriveChunkMultiple &other) const
	{
		ပြန်ပေး width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(const DriveChunkMultiple &other) const
	{
		အကယ်၍ (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		ပြန်ပေး false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;
};

sa_nracanaa DriveChunk
{
private:
	DriveType type_ = DriveType::NONE;
	sa_ngha
	{
		ကိန်းပြည့် none_;
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

	DriveBit operator[](ကိန်းပြည့် i) const
	{
		switch (type_)
		{
			pak_sa DriveType::NONE:
				ပြန်ပေး DriveBit();
			pak_sa DriveType::CONSTANT:
				ပြန်ပေး constant_[i];
			pak_sa DriveType::WIRE:
				ပြန်ပေး wire_[i];
			pak_sa DriveType::PORT:
				ပြန်ပေး port_[i];
			pak_sa DriveType::MARKER:
				ပြန်ပေး marker_[i];
			pak_sa DriveType::MULTIPLE:
				ပြန်ပေး multiple_[i];
		}
		log_abort();
	}

	void set_none(ကိန်းပြည့် width = 0)
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
		ပြန်ပေး *this;
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
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(Const const &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(DriveChunkWire const &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(DriveChunkPort const &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(DriveChunkMarker const &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(DriveChunkMultiple const &multiple)
	{
		set_none(multiple.size());
		အကယ်၍ (multiple.multiple().empty())
			ပြန်ပေး *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ပြန်ပေး *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		အကယ်၍ (multiple.multiple().empty())
			ပြန်ပေး *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ပြန်ပေး *this;
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
		ပြန်ပေး *this;
	}

	bool can_append(DriveBit const &bit) const;
	bool try_append(DriveBit const &bit);
	bool try_append(DriveChunk const &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	bool operator==(const DriveChunk &other) const
	{
		အကယ်၍ (type_ != other.type_)
			ပြန်ပေး false;

		switch (type_)
		{
			pak_sa DriveType::NONE:
				ပြန်ပေး true;
			pak_sa DriveType::CONSTANT:
				ပြန်ပေး constant_ == other.constant_;
			pak_sa DriveType::WIRE:
				ပြန်ပေး wire_ == other.wire_;
			pak_sa DriveType::PORT:
				ပြန်ပေး port_ == other.port_;
			pak_sa DriveType::MARKER:
				ပြန်ပေး marker_ == other.marker_;
			pak_sa DriveType::MULTIPLE:
				ပြန်ပေး multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(const DriveChunk &other) const
	{
		ပြန်ပေး !(*this == other);
	}

	bool operator<(const DriveChunk &other) const
	{
		အကယ်၍ (type_ != other.type_)
			ပြန်ပေး type_ < other.type_;
		switch (type_)
		{
			pak_sa DriveType::NONE:
				ပြန်ပေး false;
			pak_sa DriveType::CONSTANT:
				ပြန်ပေး constant_ < other.constant_;
			pak_sa DriveType::WIRE:
				ပြန်ပေး wire_ < other.wire_;
			pak_sa DriveType::PORT:
				ပြန်ပေး port_ < other.port_;
			pak_sa DriveType::MARKER:
				ပြန်ပေး marker_ < other.marker_;
			pak_sa DriveType::MULTIPLE:
				ပြန်ပေး multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() const { ပြန်ပေး type_; }

	bool is_none() const { ပြန်ပေး type_ == DriveType::NONE; }
	bool is_constant() const { ပြန်ပေး type_ == DriveType::CONSTANT; }
	bool is_wire() const { ပြန်ပေး type_ == DriveType::WIRE; }
	bool is_port() const { ပြန်ပေး type_ == DriveType::PORT; }
	bool is_marker() const { ပြန်ပေး type_ == DriveType::MARKER; }
	bool is_multiple() const { ပြန်ပေး type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); ပြန်ပေး constant_; }
	Const const &constant() const { log_assert(is_constant()); ပြန်ပေး constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); ပြန်ပေး wire_; }
	DriveChunkWire const &wire() const { log_assert(is_wire()); ပြန်ပေး wire_; }
	DriveChunkPort &port() { log_assert(is_port()); ပြန်ပေး port_; }
	DriveChunkPort const &port() const { log_assert(is_port()); ပြန်ပေး port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); ပြန်ပေး marker_; }
	DriveChunkMarker const &marker() const { log_assert(is_marker()); ပြန်ပေး marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); ပြန်ပေး multiple_; }
	DriveChunkMultiple const &multiple() const { log_assert(is_multiple()); ပြန်ပေး multiple_; }


	ကိန်းပြည့် size() const
	{
		switch (type_)
		{
			pak_sa DriveType::NONE:
				ပြန်ပေး none_;
			pak_sa DriveType::CONSTANT:
				ပြန်ပေး constant_.size();
			pak_sa DriveType::WIRE:
				ပြန်ပေး wire_.size();
			pak_sa DriveType::PORT:
				ပြန်ပေး port_.size();
			pak_sa DriveType::MARKER:
				ပြန်ပေး marker_.size();
			pak_sa DriveType::MULTIPLE:
				ပြန်ပေး multiple_.size();
		}
		log_abort();
	}
};

anta_hstha DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple const &bit)
	: width_(1)
{
	အတွက် (auto const &bit : bit.multiple())
		multiple_.emplace(bit);
}

sa_nracanaa DriveSpec
{
private:
	ကိန်းပြည့် width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned ကိန်းပြည့် hash_ = 0;
public:

	anta_hstha bool packed() const {
		ပြန်ပေး bits_.empty();
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
		အတွက် (auto const &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec const &sig)
	{
		// TODO: converting one chunk at a time would be faster
		အတွက် (auto const &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> const &chunks() const { pack(); ပြန်ပေး chunks_; }
	std::vector<DriveBit> const &bits() const { unpack(); ပြန်ပေး bits_; }

	ကိန်းပြည့် size() const { ပြန်ပေး width_; }

	void append(DriveBit const &bit);

	void append(DriveChunk const &chunk);

	void pack() const;

	void unpack() const;

	DriveBit &operator[](ကိန်းပြည့် index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		ပြန်ပေး bits_[index];
	}

	const DriveBit &operator[](ကိန်းပြည့် index) const
	{
		log_assert(index >= 0 && index < size());
		unpack();
		ပြန်ပေး bits_[index];
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
		ပြန်ပေး *this;
	}

	DriveSpec &operator=(DriveChunkWire const &chunk) { ပြန်ပေး *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort const &chunk) { ပြန်ပေး *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker const &chunk) { ပြန်ပေး *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple const &chunk) { ပြန်ပေး *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit const &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		ပြန်ပေး *this;
	}

	DriveSpec &operator=(DriveBitWire const &bit) { ပြန်ပေး *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort const &bit) { ပြန်ပေး *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker const &bit) { ပြန်ပေး *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple const &bit) { ပြန်ပေး *this = DriveBit(bit); }

	void updhash() const {
		အကယ်၍ (hash_ != 0)
			ပြန်ပေး;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) const;

	bool operator==(DriveSpec const &other) const {
		updhash();
		other.updhash();
		အကယ်၍ (size() != other.size() || hash_ != other.hash_)
			ပြန်ပေး false;
		ပြန်ပေး chunks() == other.chunks();
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
		ကိန်းပြည့် id = -1;

		DriveBitId() {};

		DriveBitId(ကိန်းပြည့် id) : id(id) { }

		bool operator==(const DriveBitId &other) const { ပြန်ပေး id == other.id; }
		bool operator!=(const DriveBitId &other) const { ပြန်ပေး id != other.id; }
		bool operator<(const DriveBitId &other) const { ပြန်ပေး id < other.id; }
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
		ကိန်းပြည့် count(DriveBitId src);

		DriveBitId at(DriveBitId src, ကိန်းပြည့် index);
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
	ကိန်းပြည့် next_offset = 1 + (ကိန်းပြည့်)State::Sm;

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
		ပြန်ပေး
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
		အတွက် (ကိန်းပြည့် i = 0; i != GetSize(a); ++i)
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
	dict<DriveBitId, ကိန်းပြည့်> orient_undirected_distance;

	void orient_undirected(DriveBitId id);

public:
	DriveBit operator()(DriveBit const &bit);

	DriveSpec operator()(DriveSpec spec);

private:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		ပြန်ပေး wire->has_attribute(ID(keep));
	}
};

anta_hstha Hasher DriveBitWire::hash_into(Hasher h) const
{
	h.eat(wire->name);
	h.eat(offset);
	ပြန်ပေး h;
}

anta_hstha Hasher DriveBitPort::hash_into(Hasher h) const
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	ပြန်ပေး h;
}

anta_hstha Hasher DriveBitMarker::hash_into(Hasher h) const
{
	h.eat(marker);
	h.eat(offset);
	ပြန်ပေး h;
}

anta_hstha Hasher DriveBitMultiple::hash_into(Hasher h) const
{
	h.eat(multiple_);
	ပြန်ပေး h;
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
	ပြန်ပေး h;
}

anta_hstha Hasher DriveChunkWire::hash_into(Hasher h) const
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	ပြန်ပေး h;
}

anta_hstha Hasher DriveChunkPort::hash_into(Hasher h) const
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	ပြန်ပေး h;
}

anta_hstha Hasher DriveChunkMarker::hash_into(Hasher h) const
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	ပြန်ပေး h;
}

anta_hstha Hasher DriveChunkMultiple::hash_into(Hasher h) const
{
	h.eat(width_);
	h.eat(multiple_);
	ပြန်ပေး h;
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
	ပြန်ပေး h;
}

anta_hstha Hasher DriveSpec::hash_into(Hasher h) const
{
	updhash();
	h.eat(hash_);
	ပြန်ပေး h;
}

anta_hstha Hasher DriverMap::DriveBitId::hash_into(Hasher h) const
{
	h.eat(id);
	ပြန်ပေး h;
}

YOSYS_NAMESPACE_END

#endif
