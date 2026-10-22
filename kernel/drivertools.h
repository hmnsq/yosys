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

#اگر_تعریف_نشده DRIVERTOOLS_H
#تعریف_کن DRIVERTOOLS_H

#ضمن <string>
#ضمن <type_traits>

#ضمن "kernel/rtlil.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

بنية Drive_بیت;

بنية DriveChunkWire;
بنية Drive_Chunk_پورت;
بنية DriveChunkMarker;
بنية DriveChunk;

بنية DriveSpec;

std::string log_signal(DriveChunkWire ثابت &chunk);
std::string log_signal(Drive_Chunk_پورت ثابت &chunk);
std::string log_signal(DriveChunkMarker ثابت &chunk);
std::string log_signal(DriveChunk ثابت &chunk);
std::string log_signal(DriveSpec ثابت &chunk);

تعداد class DriveType : بیعلامت حرف
{
	NONE,
	CONSTANT,
	WIRE,
	پورت_2,
	مضرب,
	MARKER,
};

بنية Drive_بیت_Wire
{
	Wire *wire;
	صحيح جابه_جایی;

	Drive_بیت_Wire(Wire *wire, صحيح جابه_جایی) : wire(wire), جابه_جایی(جابه_جایی) {}

	bool عملگر==(ثابت Drive_بیت_Wire &other) ثابت
	{
		أرجع wire == other.wire && جابه_جایی == other.جابه_جایی;
	}

	bool عملگر<(ثابت Drive_بیت_Wire &other) ثابت
	{
		إذا (wire != other.wire)
			أرجع wire->name < other.wire->name;
		أرجع جابه_جایی < other.جابه_جایی;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;


	عملگر Sig_بیت() ثابت
	{
		أرجع Sig_بیت(wire, جابه_جایی);
	}
};

بنية Drive_بیت_پورت
{
	Cell *cell;
	IdString پورت;
	صحيح جابه_جایی;

	Drive_بیت_پورت(Cell *cell, IdString پورت, صحيح جابه_جایی) : cell(cell), پورت(پورت), جابه_جایی(جابه_جایی) {}

	bool عملگر==(ثابت Drive_بیت_پورت &other) ثابت
	{
		أرجع cell == other.cell && پورت == other.پورت && جابه_جایی == other.جابه_جایی;
	}

	bool عملگر<(ثابت Drive_بیت_پورت &other) ثابت
	{
		إذا (cell != other.cell)
			أرجع cell->name < other.cell->name;
		إذا (پورت != other.پورت)
			أرجع پورت < other.پورت;
		أرجع جابه_جایی < other.جابه_جایی;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};


بنية Drive_بیت_Marker
{
	صحيح marker;
	صحيح جابه_جایی;

	Drive_بیت_Marker(صحيح marker, صحيح جابه_جایی) : marker(marker), جابه_جایی(جابه_جایی) {}

	bool عملگر==(ثابت Drive_بیت_Marker &other) ثابت
	{
		أرجع marker == other.marker && جابه_جایی == other.جابه_جایی;
	}

	bool عملگر<(ثابت Drive_بیت_Marker &other) ثابت
	{
		إذا (marker != other.marker)
			أرجع marker < other.marker;
		أرجع جابه_جایی < other.جابه_جایی;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};

بنية Drive_بیت_مضرب
{
خاص:
	pool<Drive_بیت> مضرب_3;

عام:
	Drive_بیت_مضرب();
	Drive_بیت_مضرب(Drive_بیت ثابت &single);

	pool<Drive_بیت> ثابت &مضرب_2() ثابت { أرجع مضرب_3; }

	فراغ ادغام(Drive_بیت_مضرب ثابت &other)
	{
		لكل (Drive_بیت ثابت &single : other.مضرب_3)
			ادغام(single);
	}

	فراغ ادغام(Drive_بیت_مضرب &&other)
	{
		لكل (Drive_بیت &single : other.مضرب_3)
			ادغام(std::move(single));
	}

	فراغ ادغام(Drive_بیت ثابت &single);
	فراغ ادغام(Drive_بیت &&single);

	bool عملگر==(ثابت Drive_بیت_مضرب &other) ثابت
	{
		أرجع مضرب_3 == other.مضرب_3;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Drive_بیت
{
خاص:
	DriveType type_ = DriveType::NONE;
	اتحاد
	{
		State constant_;
		Drive_بیت_Wire wire_;
		Drive_بیت_پورت پورت_3;
		Drive_بیت_Marker marker_;
		Drive_بیت_مضرب مضرب_3;
	};
عام:
	Drive_بیت()  {}

	Drive_بیت(Sig_بیت ثابت &بیت);

	Drive_بیت(Drive_بیت ثابت &other) { *this = other; }
	Drive_بیت(Drive_بیت &&other) { *this = other; }


	Drive_بیت(State constant) { *this = constant; }
	Drive_بیت(Drive_بیت_Wire ثابت &wire) { *this = wire; }
	Drive_بیت(Drive_بیت_Wire &&wire) { *this = wire; }
	Drive_بیت(Drive_بیت_پورت ثابت &پورت) { *this = پورت; }
	Drive_بیت(Drive_بیت_پورت &&پورت) { *this = پورت; }
	Drive_بیت(Drive_بیت_Marker ثابت &marker) { *this = marker; }
	Drive_بیت(Drive_بیت_Marker &&marker) { *this = marker; }
	Drive_بیت(Drive_بیت_مضرب ثابت &مضرب_2) { *this = مضرب_2; }
	Drive_بیت(Drive_بیت_مضرب &&مضرب_2) { *this = مضرب_2; }

	~Drive_بیت() { مجموعه_none(); }

	فراغ مجموعه_none()
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				اكسر;
			حالة DriveType::CONSTANT:
				اكسر;
			حالة DriveType::WIRE:
				wire_.~Drive_بیت_Wire();
				اكسر;
			حالة DriveType::پورت_2:
				پورت_3.~Drive_بیت_پورت();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~Drive_بیت_Marker();
				اكسر;
			حالة DriveType::مضرب:
				مضرب_3.~Drive_بیت_مضرب();
				اكسر;
		}
		type_ = DriveType::NONE;
	}

	Drive_بیت &عملگر=(Drive_بیت ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				مجموعه_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::پورت_2:
				*this = other.پورت_3;
				اكسر;
			حالة DriveType::MARKER:
				*this = other.marker_;
				اكسر;
			حالة DriveType::مضرب:
				*this = other.مضرب_3;
				اكسر;
		}
		أرجع *this;
	}

	Drive_بیت &عملگر=(Drive_بیت &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				مجموعه_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::پورت_2:
				*this = std::move(other.پورت_3);
				اكسر;
			حالة DriveType::MARKER:
				*this = std::move(other.marker_);
				اكسر;
			حالة DriveType::مضرب:
				*this = std::move(other.مضرب_3);
				اكسر;
		}
		أرجع *this;
	}

	Drive_بیت &عملگر=(State constant)
	{
		مجموعه_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	Drive_بیت &عملگر=(Drive_بیت_Wire ثابت &wire)
	{
		مجموعه_none();
		new (&wire_) Drive_بیت_Wire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	Drive_بیت &عملگر=(Drive_بیت_Wire &&wire)
	{
		مجموعه_none();
		new (&wire_) Drive_بیت_Wire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	Drive_بیت &عملگر=(Drive_بیت_پورت ثابت &پورت)
	{
		مجموعه_none();
		new (&پورت_3) Drive_بیت_پورت(پورت);
		type_ = DriveType::پورت_2;
		أرجع *this;
	}

	Drive_بیت &عملگر=(Drive_بیت_پورت &&پورت)
	{
		مجموعه_none();
		new (&پورت_3) Drive_بیت_پورت(پورت);
		type_ = DriveType::پورت_2;
		أرجع *this;
	}

	Drive_بیت &عملگر=(Drive_بیت_Marker ثابت &marker)
	{
		مجموعه_none();
		new (&marker_) Drive_بیت_Marker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	Drive_بیت &عملگر=(Drive_بیت_Marker &&marker)
	{
		مجموعه_none();
		new (&marker_) Drive_بیت_Marker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	Drive_بیت &عملگر=(Drive_بیت_مضرب ثابت &مضرب_2)
	{
		مجموعه_none();
		إذا (مضرب_2.مضرب_2().خالی())
			أرجع *this;
		new (&مضرب_3) Drive_بیت_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	Drive_بیت &عملگر=(Drive_بیت_مضرب &&مضرب_2)
	{
		مجموعه_none();
		إذا (مضرب_2.مضرب_2().خالی())
			أرجع *this;
		new (&مضرب_3) Drive_بیت_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عملگر==(ثابت Drive_بیت &other) ثابت
	{
		إذا (type_ != other.type_)
			أرجع false;

		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع true;
			حالة DriveType::CONSTANT:
				أرجع constant_ == other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ == other.wire_;
			حالة DriveType::پورت_2:
				أرجع پورت_3 == other.پورت_3;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::مضرب:
				أرجع مضرب_3 == other.مضرب_3;
		}
		log_abort();
	}

	bool عملگر!=(ثابت Drive_بیت &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool عملگر<(ثابت Drive_بیت &other) ثابت
	{
		إذا (type_ != other.type_)
			أرجع type_ < other.type_;
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع false;
			حالة DriveType::CONSTANT:
				أرجع constant_ < other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ < other.wire_;
			حالة DriveType::پورت_2:
				أرجع پورت_3 < other.پورت_3;
			حالة DriveType::MARKER:
				أرجع marker_ < other.marker_;
			حالة DriveType::مضرب:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ثابت { أرجع type_; }

	bool is_none() ثابت { أرجع type_ == DriveType::NONE; }
	bool is_constant() ثابت { أرجع type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { أرجع type_ == DriveType::WIRE; }
	bool is_پورت() ثابت { أرجع type_ == DriveType::پورت_2; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_مضرب() ثابت { أرجع type_ == DriveType::مضرب; }

	State &constant() { log_assert(is_constant()); أرجع constant_; }
	State ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	Drive_بیت_Wire &wire() { log_assert(is_wire()); أرجع wire_; }
	Drive_بیت_Wire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	Drive_بیت_پورت &پورت() { log_assert(is_پورت()); أرجع پورت_3; }
	Drive_بیت_پورت ثابت &پورت() ثابت { log_assert(is_پورت()); أرجع پورت_3; }
	Drive_بیت_Marker &marker() { log_assert(is_marker()); أرجع marker_; }
	Drive_بیت_Marker ثابت &marker() ثابت { log_assert(is_marker()); أرجع marker_; }
	Drive_بیت_مضرب &مضرب_2() { log_assert(is_مضرب()); أرجع مضرب_3; }
	Drive_بیت_مضرب ثابت &مضرب_2() ثابت { log_assert(is_مضرب()); أرجع مضرب_3; }

	فراغ ادغام(Drive_بیت ثابت &other);

};

مضمن Drive_بیت_مضرب::Drive_بیت_مضرب() {}
مضمن Drive_بیت_مضرب::Drive_بیت_مضرب(Drive_بیت ثابت &single)
{
	مضرب_3.emplace(single);
}

بنية DriveChunkWire
{
	Wire *wire;
	صحيح جابه_جایی;
	صحيح عرض;

	DriveChunkWire(Wire *wire, صحيح جابه_جایی, صحيح عرض) : wire(wire), جابه_جایی(جابه_جایی), عرض(عرض) {}
	DriveChunkWire(Drive_بیت_Wire ثابت &بیت) : wire(بیت.wire), جابه_جایی(بیت.جابه_جایی), عرض(1) {}

	صحيح اندازه() ثابت { أرجع عرض; }

	Drive_بیت_Wire عملگر[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < عرض);
		أرجع Drive_بیت_Wire(wire, جابه_جایی + i);
	}

	bool can_append(Drive_بیت_Wire ثابت &بیت) ثابت;
	bool try_append(Drive_بیت_Wire ثابت &بیت);
	bool try_append(DriveChunkWire ثابت &chunk);

	// Whether this chunk is a whole wire
	bool is_کامل() ثابت { أرجع جابه_جایی == 0 && عرض == wire->عرض; }

	bool عملگر==(ثابت DriveChunkWire &other) ثابت
	{
		أرجع wire == other.wire && جابه_جایی == other.جابه_جایی && عرض == other.عرض;
	}

	bool عملگر<(ثابت DriveChunkWire &other) ثابت
	{
		إذا (wire != other.wire)
			أرجع wire->name < other.wire->name;
		إذا (عرض != other.عرض)
			أرجع عرض < other.عرض;
		أرجع جابه_جایی < other.جابه_جایی;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	explicit عملگر SigChunk() ثابت
	{
		أرجع SigChunk(wire, جابه_جایی, عرض);
	}
};

بنية Drive_Chunk_پورت
{
	Cell *cell;
	IdString پورت;
	صحيح جابه_جایی;
	صحيح عرض;

	Drive_Chunk_پورت(Cell *cell, IdString پورت, صحيح جابه_جایی, صحيح عرض) :
		cell(cell), پورت(پورت), جابه_جایی(جابه_جایی), عرض(عرض) { }
	Drive_Chunk_پورت(Cell *cell, IdString پورت) :
		cell(cell), پورت(پورت), جابه_جایی(0), عرض(Get_اندازه(cell->connections().at(پورت))) { }
	Drive_Chunk_پورت(Cell *cell, std::pair<IdString, SigSpec> ثابت &conn) :
		cell(cell), پورت(conn.اولی), جابه_جایی(0), عرض(Get_اندازه(conn.دومی)) { }
	Drive_Chunk_پورت(Drive_بیت_پورت ثابت &بیت) :
		cell(بیت.cell), پورت(بیت.پورت), جابه_جایی(بیت.جابه_جایی), عرض(1) { }

	صحيح اندازه() ثابت { أرجع عرض; }

	Drive_بیت_پورت عملگر[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < عرض);
		أرجع Drive_بیت_پورت(cell, پورت, جابه_جایی + i);
	}

	bool can_append(Drive_بیت_پورت ثابت &بیت) ثابت;
	bool try_append(Drive_بیت_پورت ثابت &بیت);
	bool try_append(Drive_Chunk_پورت ثابت &chunk);

	// Whether this chunk is a whole port
	bool is_کامل() ثابت
	{
		أرجع جابه_جایی == 0 && عرض == cell->connections().at(پورت).اندازه();
	}

	bool عملگر==(ثابت Drive_Chunk_پورت &other) ثابت
	{
		أرجع cell == other.cell && پورت == other.پورت && جابه_جایی == other.جابه_جایی && عرض == other.عرض;
	}

	bool عملگر<(ثابت Drive_Chunk_پورت &other) ثابت
	{
		إذا (cell != other.cell)
			أرجع cell->name < other.cell->name;
		إذا (پورت != other.پورت)
			أرجع پورت < other.پورت;
		إذا (عرض != other.عرض)
			أرجع عرض < other.عرض;
		أرجع جابه_جایی < other.جابه_جایی;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};


بنية DriveChunkMarker
{
	صحيح marker;
	صحيح جابه_جایی;
	صحيح عرض;

	DriveChunkMarker(صحيح marker, صحيح جابه_جایی, صحيح عرض) :
		marker(marker), جابه_جایی(جابه_جایی), عرض(عرض) {}
	DriveChunkMarker(Drive_بیت_Marker ثابت &بیت) :
		marker(بیت.marker), جابه_جایی(بیت.جابه_جایی), عرض(1) {}

	صحيح اندازه() ثابت { أرجع عرض; }

	Drive_بیت_Marker عملگر[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < عرض);
		أرجع Drive_بیت_Marker(marker, جابه_جایی + i);
	}

	bool can_append(Drive_بیت_Marker ثابت &بیت) ثابت;
	bool try_append(Drive_بیت_Marker ثابت &بیت);
	bool try_append(DriveChunkMarker ثابت &chunk);

	bool عملگر==(ثابت DriveChunkMarker &other) ثابت
	{
		أرجع marker == other.marker && جابه_جایی == other.جابه_جایی && عرض == other.عرض;
	}

	bool عملگر<(ثابت DriveChunkMarker &other) ثابت
	{
		إذا (marker != other.marker)
			أرجع marker < other.marker;
		إذا (عرض != other.عرض)
			أرجع عرض < other.عرض;
		أرجع جابه_جایی < other.جابه_جایی;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Drive_Chunk_مضرب
{
خاص:
	mutable pool<DriveChunk> مضرب_3;
	صحيح عرض_3;

عام:
	pool<DriveChunk> ثابت &مضرب_2() ثابت { أرجع مضرب_3; }

	Drive_Chunk_مضرب(Drive_بیت_مضرب ثابت &بیت);

	صحيح اندازه() ثابت { أرجع عرض_3; }

	Drive_بیت_مضرب عملگر[](صحيح i) ثابت;

	bool can_append(Drive_بیت_مضرب ثابت &بیت) ثابت;

	bool try_append(Drive_بیت_مضرب ثابت &بیت);


	bool can_append(Drive_Chunk_مضرب ثابت &بیت) ثابت;

	bool try_append(Drive_Chunk_مضرب ثابت &بیت);

	bool عملگر==(ثابت Drive_Chunk_مضرب &other) ثابت
	{
		أرجع عرض_3 == other.عرض_3 && مضرب_3 == other.مضرب_3;
	}

	bool عملگر<(ثابت Drive_Chunk_مضرب &other) ثابت
	{
		إذا (مضرب_3.اندازه() < other.مضرب_3.اندازه())

		مضرب_3.مرتب_سازی();
		أرجع false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية DriveChunk
{
خاص:
	DriveType type_ = DriveType::NONE;
	اتحاد
	{
		صحيح none_;
		Const constant_;
		DriveChunkWire wire_;
		Drive_Chunk_پورت پورت_3;
		DriveChunkMarker marker_;
		Drive_Chunk_مضرب مضرب_3;
	};

عام:
	DriveChunk() { مجموعه_none(); }

	DriveChunk(DriveChunk ثابت &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(Drive_بیت ثابت &other) { *this = other; }

	DriveChunk(Const ثابت &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ثابت &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(Drive_Chunk_پورت ثابت &پورت) { *this = پورت; }
	DriveChunk(Drive_Chunk_پورت &&پورت) { *this = پورت; }
	DriveChunk(DriveChunkMarker ثابت &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(Drive_Chunk_مضرب ثابت &مضرب_2) { *this = مضرب_2; }
	DriveChunk(Drive_Chunk_مضرب &&مضرب_2) { *this = مضرب_2; }

	~DriveChunk() { مجموعه_none(); }

	Drive_بیت عملگر[](صحيح i) ثابت
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع Drive_بیت();
			حالة DriveType::CONSTANT:
				أرجع constant_[i];
			حالة DriveType::WIRE:
				أرجع wire_[i];
			حالة DriveType::پورت_2:
				أرجع پورت_3[i];
			حالة DriveType::MARKER:
				أرجع marker_[i];
			حالة DriveType::مضرب:
				أرجع مضرب_3[i];
		}
		log_abort();
	}

	فراغ مجموعه_none(صحيح عرض = 0)
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				none_ = عرض;
				اكسر;
			حالة DriveType::CONSTANT:
				constant_.~Const();
				اكسر;
			حالة DriveType::WIRE:
				wire_.~DriveChunkWire();
				اكسر;
			حالة DriveType::پورت_2:
				پورت_3.~Drive_Chunk_پورت();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~DriveChunkMarker();
				اكسر;
			حالة DriveType::مضرب:
				مضرب_3.~Drive_Chunk_مضرب();
				اكسر;
		}
		type_ = DriveType::NONE;
		none_ = عرض;
	}

	DriveChunk &عملگر=(DriveChunk ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				مجموعه_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::پورت_2:
				*this = other.پورت_3;
				اكسر;
			حالة DriveType::MARKER:
				*this = other.marker_;
				اكسر;
			حالة DriveType::مضرب:
				*this = other.مضرب_3;
				اكسر;
		}
		أرجع *this;
	}

	DriveChunk &عملگر=(DriveChunk &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				مجموعه_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::پورت_2:
				*this = std::move(other.پورت_3);
				اكسر;
			حالة DriveType::MARKER:
				*this = std::move(other.marker_);
				اكسر;
			حالة DriveType::مضرب:
				*this = std::move(other.مضرب_3);
				اكسر;
		}
		أرجع *this;
	}

	DriveChunk &عملگر=(Const ثابت &constant)
	{
		مجموعه_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &عملگر=(Const &&constant)
	{
		مجموعه_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &عملگر=(DriveChunkWire ثابت &wire)
	{
		مجموعه_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &عملگر=(DriveChunkWire &&wire)
	{
		مجموعه_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &عملگر=(Drive_Chunk_پورت ثابت &پورت)
	{
		مجموعه_none();
		new (&پورت_3) Drive_Chunk_پورت(پورت);
		type_ = DriveType::پورت_2;
		أرجع *this;
	}

	DriveChunk &عملگر=(Drive_Chunk_پورت &&پورت)
	{
		مجموعه_none();
		new (&پورت_3) Drive_Chunk_پورت(پورت);
		type_ = DriveType::پورت_2;
		أرجع *this;
	}

	DriveChunk &عملگر=(DriveChunkMarker ثابت &marker)
	{
		مجموعه_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &عملگر=(DriveChunkMarker &&marker)
	{
		مجموعه_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &عملگر=(Drive_Chunk_مضرب ثابت &مضرب_2)
	{
		مجموعه_none(مضرب_2.اندازه());
		إذا (مضرب_2.مضرب_2().خالی())
			أرجع *this;
		new (&مضرب_3) Drive_Chunk_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	DriveChunk &عملگر=(Drive_Chunk_مضرب &&مضرب_2)
	{
		مجموعه_none(مضرب_2.اندازه());
		إذا (مضرب_2.مضرب_2().خالی())
			أرجع *this;
		new (&مضرب_3) Drive_Chunk_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	DriveChunk &عملگر=(Drive_بیت ثابت &other)
	{
		بدل (other.type())
		{
			حالة DriveType::NONE:
				مجموعه_none(1);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = Const(other.constant());
				اكسر;
			حالة DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				اكسر;
			حالة DriveType::پورت_2:
				*this = Drive_Chunk_پورت(other.پورت());
				اكسر;
			حالة DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				اكسر;
			حالة DriveType::مضرب:
				*this = Drive_Chunk_مضرب(other.مضرب_2());
				اكسر;
		}
		أرجع *this;
	}

	bool can_append(Drive_بیت ثابت &بیت) ثابت;
	bool try_append(Drive_بیت ثابت &بیت);
	bool try_append(DriveChunk ثابت &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عملگر==(ثابت DriveChunk &other) ثابت
	{
		إذا (type_ != other.type_)
			أرجع false;

		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع true;
			حالة DriveType::CONSTANT:
				أرجع constant_ == other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ == other.wire_;
			حالة DriveType::پورت_2:
				أرجع پورت_3 == other.پورت_3;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::مضرب:
				أرجع مضرب_3 == other.مضرب_3;
		}
		log_abort();
	}

	bool عملگر!=(ثابت DriveChunk &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool عملگر<(ثابت DriveChunk &other) ثابت
	{
		إذا (type_ != other.type_)
			أرجع type_ < other.type_;
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع false;
			حالة DriveType::CONSTANT:
				أرجع constant_ < other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ < other.wire_;
			حالة DriveType::پورت_2:
				أرجع پورت_3 < other.پورت_3;
			حالة DriveType::MARKER:
				أرجع marker_ < other.marker_;
			حالة DriveType::مضرب:
				أرجع مضرب_3 < other.مضرب_3;
		}
		log_abort();
	}

	DriveType type() ثابت { أرجع type_; }

	bool is_none() ثابت { أرجع type_ == DriveType::NONE; }
	bool is_constant() ثابت { أرجع type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { أرجع type_ == DriveType::WIRE; }
	bool is_پورت() ثابت { أرجع type_ == DriveType::پورت_2; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_مضرب() ثابت { أرجع type_ == DriveType::مضرب; }

	Const &constant() { log_assert(is_constant()); أرجع constant_; }
	Const ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); أرجع wire_; }
	DriveChunkWire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	Drive_Chunk_پورت &پورت() { log_assert(is_پورت()); أرجع پورت_3; }
	Drive_Chunk_پورت ثابت &پورت() ثابت { log_assert(is_پورت()); أرجع پورت_3; }
	DriveChunkMarker &marker() { log_assert(is_marker()); أرجع marker_; }
	DriveChunkMarker ثابت &marker() ثابت { log_assert(is_marker()); أرجع marker_; }
	Drive_Chunk_مضرب &مضرب_2() { log_assert(is_مضرب()); أرجع مضرب_3; }
	Drive_Chunk_مضرب ثابت &مضرب_2() ثابت { log_assert(is_مضرب()); أرجع مضرب_3; }


	صحيح اندازه() ثابت
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع none_;
			حالة DriveType::CONSTANT:
				أرجع constant_.اندازه();
			حالة DriveType::WIRE:
				أرجع wire_.اندازه();
			حالة DriveType::پورت_2:
				أرجع پورت_3.اندازه();
			حالة DriveType::MARKER:
				أرجع marker_.اندازه();
			حالة DriveType::مضرب:
				أرجع مضرب_3.اندازه();
		}
		log_abort();
	}
};

مضمن Drive_Chunk_مضرب::Drive_Chunk_مضرب(Drive_بیت_مضرب ثابت &بیت)
	: عرض_3(1)
{
	لكل (خودکار ثابت &بیت : بیت.مضرب_2())
		مضرب_3.emplace(بیت);
}

بنية DriveSpec
{
خاص:
	صحيح عرض_3 = 0;
	mutable std::وکتور<DriveChunk> chunks_;
	mutable std::وکتور<Drive_بیت> بیتها_2;
	mutable بیعلامت صحيح hash_ = 0;
عام:

	مضمن bool packed() ثابت {
		أرجع بیتها_2.خالی();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ثابت &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_پورت ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ثابت &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_مضرب ثابت &chunk) { *this = chunk; }

	DriveSpec(Drive_بیت ثابت &بیت) { *this = بیت; }
	DriveSpec(Drive_بیت_Wire ثابت &بیت) { *this = بیت; }
	DriveSpec(Drive_بیت_پورت ثابت &بیت) { *this = بیت; }
	DriveSpec(Drive_بیت_Marker ثابت &بیت) { *this = بیت; }
	DriveSpec(Drive_بیت_مضرب ثابت &بیت) { *this = بیت; }

	DriveSpec(std::وکتور<DriveChunk> ثابت &chunks) : chunks_(chunks) { compute_عرض(); }

	DriveSpec(std::وکتور<Drive_بیت> ثابت &بیتها)
	{
		لكل (خودکار ثابت &بیت : بیتها)
			append(بیت);
	}

	DriveSpec(SigSpec ثابت &sig)
	{
		// TODO: converting one chunk at a time would be faster
		لكل (خودکار ثابت &بیت : sig.بیتها())
			append(بیت);
	}

	std::وکتور<DriveChunk> ثابت &chunks() ثابت { pack(); أرجع chunks_; }
	std::وکتور<Drive_بیت> ثابت &بیتها() ثابت { unpack(); أرجع بیتها_2; }

	صحيح اندازه() ثابت { أرجع عرض_3; }

	فراغ append(Drive_بیت ثابت &بیت);

	فراغ append(DriveChunk ثابت &chunk);

	فراغ pack() ثابت;

	فراغ unpack() ثابت;

	Drive_بیت &عملگر[](صحيح شاخص)
	{
		log_assert(شاخص >= 0 && شاخص < اندازه());
		unpack();
		أرجع بیتها_2[شاخص];
	}

	ثابت Drive_بیت &عملگر[](صحيح شاخص) ثابت
	{
		log_assert(شاخص >= 0 && شاخص < اندازه());
		unpack();
		أرجع بیتها_2[شاخص];
	}

	فراغ clear()
	{
		chunks_.clear();
		بیتها_2.clear();
		عرض_3 = 0;
	}

	DriveSpec &عملگر=(DriveChunk ثابت &chunk)
	{
		chunks_.clear();
		بیتها_2.clear();
		append(chunk);
		أرجع *this;
	}

	DriveSpec &عملگر=(DriveChunkWire ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &عملگر=(Drive_Chunk_پورت ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &عملگر=(DriveChunkMarker ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &عملگر=(Drive_Chunk_مضرب ثابت &chunk) { أرجع *this = DriveChunk(chunk); }

	DriveSpec &عملگر=(Drive_بیت ثابت &بیت)
	{
		chunks_.clear();
		بیتها_2.clear();
		append(بیت);
		أرجع *this;
	}

	DriveSpec &عملگر=(Drive_بیت_Wire ثابت &بیت) { أرجع *this = Drive_بیت(بیت); }
	DriveSpec &عملگر=(Drive_بیت_پورت ثابت &بیت) { أرجع *this = Drive_بیت(بیت); }
	DriveSpec &عملگر=(Drive_بیت_Marker ثابت &بیت) { أرجع *this = Drive_بیت(بیت); }
	DriveSpec &عملگر=(Drive_بیت_مضرب ثابت &بیت) { أرجع *this = Drive_بیت(بیت); }

	فراغ updhash() ثابت {
		إذا (hash_ != 0)
			أرجع;
		pack();
		hash_ = پیاپی_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عملگر==(DriveSpec ثابت &other) ثابت {
		updhash();
		other.updhash();
		إذا (اندازه() != other.اندازه() || hash_ != other.hash_)
			أرجع false;
		أرجع chunks() == other.chunks();
	}

خاص:
	فراغ compute_عرض();
};



بنية DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.راهاندازی(); }
	DriverMap(Design *design) { celltypes.راهاندازی(design); }

خاص:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	بنية Drive_بیت_Id
	{
		صحيح id = -1;

		Drive_بیت_Id() {};

		Drive_بیت_Id(صحيح id) : id(id) { }

		bool عملگر==(ثابت Drive_بیت_Id &other) ثابت { أرجع id == other.id; }
		bool عملگر!=(ثابت Drive_بیت_Id &other) ثابت { أرجع id != other.id; }
		bool عملگر<(ثابت Drive_بیت_Id &other) ثابت { أرجع id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	بنية Drive_بیت_گراف {
		dict<Drive_بیت_Id, Drive_بیت_Id> اولی_یالها;
		dict<Drive_بیت_Id, Drive_بیت_Id> دومی_یالها;
		dict<Drive_بیت_Id, pool<Drive_بیت_Id>> بیشتر_یالها;

		فراغ جمع_کن_یال(Drive_بیت_Id src, Drive_بیت_Id dst);
		Drive_بیت_Id برداشت_یال(Drive_بیت_Id src);
		فراغ clear(Drive_بیت_Id src);
		bool شامل_است(Drive_بیت_Id src);
		صحيح شمار(Drive_بیت_Id src);

		Drive_بیت_Id at(Drive_بیت_Id src, صحيح شاخص);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, Drive_بیت_Id> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, Drive_بیت_Id> پورت_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<Drive_بیت_Id, Drive_بیت> drive_بیتها;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<Drive_بیت_Id, Drive_بیت> isolated_drive_بیتها;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	صحيح بعدی_جابه_جایی = 1 + (صحيح)State::Sm;

	// Union-Find over directly connected bits that share the same single
	// driver or are undriven. We never merge connections between drivers
	// and/or kept wires.
	mfp<Drive_بیت_Id> same_driver;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally unambiguous (one side only drives or requires a driven value).
	Drive_بیت_گراف connected_drivers;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally ambiguous. Every such ambiguous connection is also present in
	// the reverse direction and has to be resolved when querying drivers.
	Drive_بیت_گراف connected_undirected;

	// Subset of `connected_undirected` for caching the resolved driving
	// direction. In case multiple drivers are present this can still contain
	// both orientations of a single connection, but for a single driver only
	// one will be present.
	Drive_بیت_گراف connected_oriented;

	// Stores for which bits we already resolved the orientation (cached in
	// `connected_oriented`).
	pool<Drive_بیت_Id> oriented_present;


	تعداد class بیت_نما {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	بیت_نما بیت_نما_2(Drive_بیت ثابت &بیت);
	Drive_بیت_Id id_from_drive_بیت(Drive_بیت ثابت &بیت);
	Drive_بیت drive_بیت_from_id(Drive_بیت_Id id);

	فراغ connect_directed_ادغام(Drive_بیت_Id driven_id, Drive_بیت_Id driver_id);
	فراغ connect_directed_میانگیر(Drive_بیت_Id driven_id, Drive_بیت_Id driver_id);
	فراغ connect_undirected(Drive_بیت_Id a_id, Drive_بیت_Id b_id);

عام:

	فراغ جمع_کن(Module *module);

	// Add a single bit connection to the driver map.
	فراغ جمع_کن(Drive_بیت ثابت &a, Drive_بیت ثابت &b);

	قالب<typename T>
	ساكن constexpr bool is_sig_type() {
		أرجع
			std::is_same<T, SigSpec>::مقدار ||
			std::is_same<T, SigChunk>::مقدار ||
			std::is_same<T, DriveSpec>::مقدار ||
			std::is_same<T, DriveChunk>::مقدار ||
			std::is_same<T, Drive_Chunk_پورت>::مقدار ||
			std::is_same<T, DriveChunkWire>::مقدار ||
			std::is_same<T, Const>::مقدار;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	قالب<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	جمع_کن(T ثابت &a, U ثابت &b)
	{
		log_assert(a.اندازه() == b.اندازه());
		لكل (صحيح i = 0; i != Get_اندازه(a); ++i)
			جمع_کن(Drive_بیت(a[i]), Drive_بیت(b[i]));
	}


	// Specialized version that avoids unpacking
	فراغ جمع_کن(SigSpec ثابت &a, SigSpec ثابت &b);

خاص:
	فراغ جمع_کن_پورت(Cell *cell, IdString پورت, SigSpec ثابت &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<Drive_بیت_Id> orient_undirected_دیده;
	pool<Drive_بیت_Id> orient_undirected_drivers;
	dict<Drive_بیت_Id, صحيح> orient_undirected_فاصله;

	فراغ orient_undirected(Drive_بیت_Id id);

عام:
	Drive_بیت عملگر()(Drive_بیت ثابت &بیت);

	DriveSpec عملگر()(DriveSpec spec);

خاص:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		أرجع wire->has_attribute(ID(keep));
	}
};

مضمن Hasher Drive_بیت_Wire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(جابه_جایی);
	أرجع h;
}

مضمن Hasher Drive_بیت_پورت::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورت);
	h.eat(جابه_جایی);
	أرجع h;
}

مضمن Hasher Drive_بیت_Marker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(جابه_جایی);
	أرجع h;
}

مضمن Hasher Drive_بیت_مضرب::hash_into(Hasher h) ثابت
{
	h.eat(مضرب_3);
	أرجع h;
}

مضمن Hasher Drive_بیت::hash_into(Hasher h) ثابت
{
	بدل (type_) {
	حالة DriveType::NONE:
		h.eat(0);
		اكسر;
	حالة DriveType::CONSTANT:
		h.eat(constant_);
		اكسر;
	حالة DriveType::WIRE:
		h.eat(wire_);
		اكسر;
	حالة DriveType::پورت_2:
		h.eat(پورت_3);
		اكسر;
	حالة DriveType::MARKER:
		h.eat(marker_);
		اكسر;
	حالة DriveType::مضرب:
		h.eat(مضرب_3);
		اكسر;
	}
	h.eat(type_);
	أرجع h;
}

مضمن Hasher DriveChunkWire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(عرض);
	h.eat(جابه_جایی);
	أرجع h;
}

مضمن Hasher Drive_Chunk_پورت::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورت);
	h.eat(عرض);
	h.eat(جابه_جایی);
	أرجع h;
}

مضمن Hasher DriveChunkMarker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(عرض);
	h.eat(جابه_جایی);
	أرجع h;
}

مضمن Hasher Drive_Chunk_مضرب::hash_into(Hasher h) ثابت
{
	h.eat(عرض_3);
	h.eat(مضرب_3);
	أرجع h;
}

مضمن Hasher DriveChunk::hash_into(Hasher h) ثابت
{
	بدل (type_) {
	حالة DriveType::NONE:
		h.eat(0);
		اكسر;
	حالة DriveType::CONSTANT:
		h.eat(constant_);
		اكسر;
	حالة DriveType::WIRE:
		h.eat(wire_);
		اكسر;
	حالة DriveType::پورت_2:
		h.eat(پورت_3);
		اكسر;
	حالة DriveType::MARKER:
		h.eat(marker_);
		اكسر;
	حالة DriveType::مضرب:
		h.eat(مضرب_3);
		اكسر;
	}
	h.eat(type_);
	أرجع h;
}

مضمن Hasher DriveSpec::hash_into(Hasher h) ثابت
{
	updhash();
	h.eat(hash_);
	أرجع h;
}

مضمن Hasher DriverMap::Drive_بیت_Id::hash_into(Hasher h) ثابت
{
	h.eat(id);
	أرجع h;
}

YOSYS_NAMESPACE_انتها

#پایان_اگر
