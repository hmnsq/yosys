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

#که_نه_تعریف_شوی DRIVERTOOLS_H
#تعریف_کړه DRIVERTOOLS_H

#ضمن <string>
#ضمن <type_traits>

#ضمن "kernel/rtlil.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

بنية Drive_بټ;

بنية DriveChunkWire;
بنية Drive_Chunk_پورټ;
بنية DriveChunkMarker;
بنية DriveChunk;

بنية DriveSpec;

std::string log_signal(DriveChunkWire ثابت &chunk);
std::string log_signal(Drive_Chunk_پورټ ثابت &chunk);
std::string log_signal(DriveChunkMarker ثابت &chunk);
std::string log_signal(DriveChunk ثابت &chunk);
std::string log_signal(DriveSpec ثابت &chunk);

تعداد class DriveType : بینښه حرف
{
	NONE,
	CONSTANT,
	WIRE,
	پورټ_2,
	مضرب,
	MARKER,
};

بنية Drive_بټ_Wire
{
	Wire *wire;
	صحيح دننه_واټن;

	Drive_بټ_Wire(Wire *wire, صحيح دننه_واټن) : wire(wire), دننه_واټن(دننه_واټن) {}

	bool چلوونکی==(ثابت Drive_بټ_Wire &other) ثابت
	{
		أرجع wire == other.wire && دننه_واټن == other.دننه_واټن;
	}

	bool چلوونکی<(ثابت Drive_بټ_Wire &other) ثابت
	{
		إذا (wire != other.wire)
			أرجع wire->name < other.wire->name;
		أرجع دننه_واټن < other.دننه_واټن;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;


	چلوونکی Sig_بټ() ثابت
	{
		أرجع Sig_بټ(wire, دننه_واټن);
	}
};

بنية Drive_بټ_پورټ
{
	Cell *cell;
	IdString پورټ;
	صحيح دننه_واټن;

	Drive_بټ_پورټ(Cell *cell, IdString پورټ, صحيح دننه_واټن) : cell(cell), پورټ(پورټ), دننه_واټن(دننه_واټن) {}

	bool چلوونکی==(ثابت Drive_بټ_پورټ &other) ثابت
	{
		أرجع cell == other.cell && پورټ == other.پورټ && دننه_واټن == other.دننه_واټن;
	}

	bool چلوونکی<(ثابت Drive_بټ_پورټ &other) ثابت
	{
		إذا (cell != other.cell)
			أرجع cell->name < other.cell->name;
		إذا (پورټ != other.پورټ)
			أرجع پورټ < other.پورټ;
		أرجع دننه_واټن < other.دننه_واټن;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};


بنية Drive_بټ_Marker
{
	صحيح marker;
	صحيح دننه_واټن;

	Drive_بټ_Marker(صحيح marker, صحيح دننه_واټن) : marker(marker), دننه_واټن(دننه_واټن) {}

	bool چلوونکی==(ثابت Drive_بټ_Marker &other) ثابت
	{
		أرجع marker == other.marker && دننه_واټن == other.دننه_واټن;
	}

	bool چلوونکی<(ثابت Drive_بټ_Marker &other) ثابت
	{
		إذا (marker != other.marker)
			أرجع marker < other.marker;
		أرجع دننه_واټن < other.دننه_واټن;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};

بنية Drive_بټ_مضرب
{
خاص:
	pool<Drive_بټ> مضرب_3;

عام:
	Drive_بټ_مضرب();
	Drive_بټ_مضرب(Drive_بټ ثابت &single);

	pool<Drive_بټ> ثابت &مضرب_2() ثابت { أرجع مضرب_3; }

	فراغ یوځای(Drive_بټ_مضرب ثابت &other)
	{
		لكل (Drive_بټ ثابت &single : other.مضرب_3)
			یوځای(single);
	}

	فراغ یوځای(Drive_بټ_مضرب &&other)
	{
		لكل (Drive_بټ &single : other.مضرب_3)
			یوځای(std::move(single));
	}

	فراغ یوځای(Drive_بټ ثابت &single);
	فراغ یوځای(Drive_بټ &&single);

	bool چلوونکی==(ثابت Drive_بټ_مضرب &other) ثابت
	{
		أرجع مضرب_3 == other.مضرب_3;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Drive_بټ
{
خاص:
	DriveType type_ = DriveType::NONE;
	اتحاد
	{
		State constant_;
		Drive_بټ_Wire wire_;
		Drive_بټ_پورټ پورټ_3;
		Drive_بټ_Marker marker_;
		Drive_بټ_مضرب مضرب_3;
	};
عام:
	Drive_بټ()  {}

	Drive_بټ(Sig_بټ ثابت &بټ);

	Drive_بټ(Drive_بټ ثابت &other) { *this = other; }
	Drive_بټ(Drive_بټ &&other) { *this = other; }


	Drive_بټ(State constant) { *this = constant; }
	Drive_بټ(Drive_بټ_Wire ثابت &wire) { *this = wire; }
	Drive_بټ(Drive_بټ_Wire &&wire) { *this = wire; }
	Drive_بټ(Drive_بټ_پورټ ثابت &پورټ) { *this = پورټ; }
	Drive_بټ(Drive_بټ_پورټ &&پورټ) { *this = پورټ; }
	Drive_بټ(Drive_بټ_Marker ثابت &marker) { *this = marker; }
	Drive_بټ(Drive_بټ_Marker &&marker) { *this = marker; }
	Drive_بټ(Drive_بټ_مضرب ثابت &مضرب_2) { *this = مضرب_2; }
	Drive_بټ(Drive_بټ_مضرب &&مضرب_2) { *this = مضرب_2; }

	~Drive_بټ() { سېټ_none(); }

	فراغ سېټ_none()
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				اكسر;
			حالة DriveType::CONSTANT:
				اكسر;
			حالة DriveType::WIRE:
				wire_.~Drive_بټ_Wire();
				اكسر;
			حالة DriveType::پورټ_2:
				پورټ_3.~Drive_بټ_پورټ();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~Drive_بټ_Marker();
				اكسر;
			حالة DriveType::مضرب:
				مضرب_3.~Drive_بټ_مضرب();
				اكسر;
		}
		type_ = DriveType::NONE;
	}

	Drive_بټ &چلوونکی=(Drive_بټ ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سېټ_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::پورټ_2:
				*this = other.پورټ_3;
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

	Drive_بټ &چلوونکی=(Drive_بټ &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سېټ_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::پورټ_2:
				*this = std::move(other.پورټ_3);
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

	Drive_بټ &چلوونکی=(State constant)
	{
		سېټ_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	Drive_بټ &چلوونکی=(Drive_بټ_Wire ثابت &wire)
	{
		سېټ_none();
		new (&wire_) Drive_بټ_Wire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	Drive_بټ &چلوونکی=(Drive_بټ_Wire &&wire)
	{
		سېټ_none();
		new (&wire_) Drive_بټ_Wire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	Drive_بټ &چلوونکی=(Drive_بټ_پورټ ثابت &پورټ)
	{
		سېټ_none();
		new (&پورټ_3) Drive_بټ_پورټ(پورټ);
		type_ = DriveType::پورټ_2;
		أرجع *this;
	}

	Drive_بټ &چلوونکی=(Drive_بټ_پورټ &&پورټ)
	{
		سېټ_none();
		new (&پورټ_3) Drive_بټ_پورټ(پورټ);
		type_ = DriveType::پورټ_2;
		أرجع *this;
	}

	Drive_بټ &چلوونکی=(Drive_بټ_Marker ثابت &marker)
	{
		سېټ_none();
		new (&marker_) Drive_بټ_Marker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	Drive_بټ &چلوونکی=(Drive_بټ_Marker &&marker)
	{
		سېټ_none();
		new (&marker_) Drive_بټ_Marker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	Drive_بټ &چلوونکی=(Drive_بټ_مضرب ثابت &مضرب_2)
	{
		سېټ_none();
		إذا (مضرب_2.مضرب_2().تشه())
			أرجع *this;
		new (&مضرب_3) Drive_بټ_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	Drive_بټ &چلوونکی=(Drive_بټ_مضرب &&مضرب_2)
	{
		سېټ_none();
		إذا (مضرب_2.مضرب_2().تشه())
			أرجع *this;
		new (&مضرب_3) Drive_بټ_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool چلوونکی==(ثابت Drive_بټ &other) ثابت
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
			حالة DriveType::پورټ_2:
				أرجع پورټ_3 == other.پورټ_3;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::مضرب:
				أرجع مضرب_3 == other.مضرب_3;
		}
		log_abort();
	}

	bool چلوونکی!=(ثابت Drive_بټ &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool چلوونکی<(ثابت Drive_بټ &other) ثابت
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
			حالة DriveType::پورټ_2:
				أرجع پورټ_3 < other.پورټ_3;
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
	bool is_پورټ() ثابت { أرجع type_ == DriveType::پورټ_2; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_مضرب() ثابت { أرجع type_ == DriveType::مضرب; }

	State &constant() { log_assert(is_constant()); أرجع constant_; }
	State ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	Drive_بټ_Wire &wire() { log_assert(is_wire()); أرجع wire_; }
	Drive_بټ_Wire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	Drive_بټ_پورټ &پورټ() { log_assert(is_پورټ()); أرجع پورټ_3; }
	Drive_بټ_پورټ ثابت &پورټ() ثابت { log_assert(is_پورټ()); أرجع پورټ_3; }
	Drive_بټ_Marker &marker() { log_assert(is_marker()); أرجع marker_; }
	Drive_بټ_Marker ثابت &marker() ثابت { log_assert(is_marker()); أرجع marker_; }
	Drive_بټ_مضرب &مضرب_2() { log_assert(is_مضرب()); أرجع مضرب_3; }
	Drive_بټ_مضرب ثابت &مضرب_2() ثابت { log_assert(is_مضرب()); أرجع مضرب_3; }

	فراغ یوځای(Drive_بټ ثابت &other);

};

مضمن Drive_بټ_مضرب::Drive_بټ_مضرب() {}
مضمن Drive_بټ_مضرب::Drive_بټ_مضرب(Drive_بټ ثابت &single)
{
	مضرب_3.emplace(single);
}

بنية DriveChunkWire
{
	Wire *wire;
	صحيح دننه_واټن;
	صحيح پلنوالی;

	DriveChunkWire(Wire *wire, صحيح دننه_واټن, صحيح پلنوالی) : wire(wire), دننه_واټن(دننه_واټن), پلنوالی(پلنوالی) {}
	DriveChunkWire(Drive_بټ_Wire ثابت &بټ) : wire(بټ.wire), دننه_واټن(بټ.دننه_واټن), پلنوالی(1) {}

	صحيح اندازه() ثابت { أرجع پلنوالی; }

	Drive_بټ_Wire چلوونکی[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < پلنوالی);
		أرجع Drive_بټ_Wire(wire, دننه_واټن + i);
	}

	bool can_append(Drive_بټ_Wire ثابت &بټ) ثابت;
	bool try_append(Drive_بټ_Wire ثابت &بټ);
	bool try_append(DriveChunkWire ثابت &chunk);

	// Whether this chunk is a whole wire
	bool is_ټولیز() ثابت { أرجع دننه_واټن == 0 && پلنوالی == wire->پلنوالی; }

	bool چلوونکی==(ثابت DriveChunkWire &other) ثابت
	{
		أرجع wire == other.wire && دننه_واټن == other.دننه_واټن && پلنوالی == other.پلنوالی;
	}

	bool چلوونکی<(ثابت DriveChunkWire &other) ثابت
	{
		إذا (wire != other.wire)
			أرجع wire->name < other.wire->name;
		إذا (پلنوالی != other.پلنوالی)
			أرجع پلنوالی < other.پلنوالی;
		أرجع دننه_واټن < other.دننه_واټن;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	explicit چلوونکی SigChunk() ثابت
	{
		أرجع SigChunk(wire, دننه_واټن, پلنوالی);
	}
};

بنية Drive_Chunk_پورټ
{
	Cell *cell;
	IdString پورټ;
	صحيح دننه_واټن;
	صحيح پلنوالی;

	Drive_Chunk_پورټ(Cell *cell, IdString پورټ, صحيح دننه_واټن, صحيح پلنوالی) :
		cell(cell), پورټ(پورټ), دننه_واټن(دننه_واټن), پلنوالی(پلنوالی) { }
	Drive_Chunk_پورټ(Cell *cell, IdString پورټ) :
		cell(cell), پورټ(پورټ), دننه_واټن(0), پلنوالی(Get_اندازه(cell->connections().at(پورټ))) { }
	Drive_Chunk_پورټ(Cell *cell, std::pair<IdString, SigSpec> ثابت &conn) :
		cell(cell), پورټ(conn.لومړی), دننه_واټن(0), پلنوالی(Get_اندازه(conn.دوهم)) { }
	Drive_Chunk_پورټ(Drive_بټ_پورټ ثابت &بټ) :
		cell(بټ.cell), پورټ(بټ.پورټ), دننه_واټن(بټ.دننه_واټن), پلنوالی(1) { }

	صحيح اندازه() ثابت { أرجع پلنوالی; }

	Drive_بټ_پورټ چلوونکی[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < پلنوالی);
		أرجع Drive_بټ_پورټ(cell, پورټ, دننه_واټن + i);
	}

	bool can_append(Drive_بټ_پورټ ثابت &بټ) ثابت;
	bool try_append(Drive_بټ_پورټ ثابت &بټ);
	bool try_append(Drive_Chunk_پورټ ثابت &chunk);

	// Whether this chunk is a whole port
	bool is_ټولیز() ثابت
	{
		أرجع دننه_واټن == 0 && پلنوالی == cell->connections().at(پورټ).اندازه();
	}

	bool چلوونکی==(ثابت Drive_Chunk_پورټ &other) ثابت
	{
		أرجع cell == other.cell && پورټ == other.پورټ && دننه_واټن == other.دننه_واټن && پلنوالی == other.پلنوالی;
	}

	bool چلوونکی<(ثابت Drive_Chunk_پورټ &other) ثابت
	{
		إذا (cell != other.cell)
			أرجع cell->name < other.cell->name;
		إذا (پورټ != other.پورټ)
			أرجع پورټ < other.پورټ;
		إذا (پلنوالی != other.پلنوالی)
			أرجع پلنوالی < other.پلنوالی;
		أرجع دننه_واټن < other.دننه_واټن;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};


بنية DriveChunkMarker
{
	صحيح marker;
	صحيح دننه_واټن;
	صحيح پلنوالی;

	DriveChunkMarker(صحيح marker, صحيح دننه_واټن, صحيح پلنوالی) :
		marker(marker), دننه_واټن(دننه_واټن), پلنوالی(پلنوالی) {}
	DriveChunkMarker(Drive_بټ_Marker ثابت &بټ) :
		marker(بټ.marker), دننه_واټن(بټ.دننه_واټن), پلنوالی(1) {}

	صحيح اندازه() ثابت { أرجع پلنوالی; }

	Drive_بټ_Marker چلوونکی[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < پلنوالی);
		أرجع Drive_بټ_Marker(marker, دننه_واټن + i);
	}

	bool can_append(Drive_بټ_Marker ثابت &بټ) ثابت;
	bool try_append(Drive_بټ_Marker ثابت &بټ);
	bool try_append(DriveChunkMarker ثابت &chunk);

	bool چلوونکی==(ثابت DriveChunkMarker &other) ثابت
	{
		أرجع marker == other.marker && دننه_واټن == other.دننه_واټن && پلنوالی == other.پلنوالی;
	}

	bool چلوونکی<(ثابت DriveChunkMarker &other) ثابت
	{
		إذا (marker != other.marker)
			أرجع marker < other.marker;
		إذا (پلنوالی != other.پلنوالی)
			أرجع پلنوالی < other.پلنوالی;
		أرجع دننه_واټن < other.دننه_واټن;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Drive_Chunk_مضرب
{
خاص:
	mutable pool<DriveChunk> مضرب_3;
	صحيح پلنوالی_3;

عام:
	pool<DriveChunk> ثابت &مضرب_2() ثابت { أرجع مضرب_3; }

	Drive_Chunk_مضرب(Drive_بټ_مضرب ثابت &بټ);

	صحيح اندازه() ثابت { أرجع پلنوالی_3; }

	Drive_بټ_مضرب چلوونکی[](صحيح i) ثابت;

	bool can_append(Drive_بټ_مضرب ثابت &بټ) ثابت;

	bool try_append(Drive_بټ_مضرب ثابت &بټ);


	bool can_append(Drive_Chunk_مضرب ثابت &بټ) ثابت;

	bool try_append(Drive_Chunk_مضرب ثابت &بټ);

	bool چلوونکی==(ثابت Drive_Chunk_مضرب &other) ثابت
	{
		أرجع پلنوالی_3 == other.پلنوالی_3 && مضرب_3 == other.مضرب_3;
	}

	bool چلوونکی<(ثابت Drive_Chunk_مضرب &other) ثابت
	{
		إذا (مضرب_3.اندازه() < other.مضرب_3.اندازه())

		مضرب_3.ترتیبول();
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
		Drive_Chunk_پورټ پورټ_3;
		DriveChunkMarker marker_;
		Drive_Chunk_مضرب مضرب_3;
	};

عام:
	DriveChunk() { سېټ_none(); }

	DriveChunk(DriveChunk ثابت &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(Drive_بټ ثابت &other) { *this = other; }

	DriveChunk(Const ثابت &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ثابت &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(Drive_Chunk_پورټ ثابت &پورټ) { *this = پورټ; }
	DriveChunk(Drive_Chunk_پورټ &&پورټ) { *this = پورټ; }
	DriveChunk(DriveChunkMarker ثابت &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(Drive_Chunk_مضرب ثابت &مضرب_2) { *this = مضرب_2; }
	DriveChunk(Drive_Chunk_مضرب &&مضرب_2) { *this = مضرب_2; }

	~DriveChunk() { سېټ_none(); }

	Drive_بټ چلوونکی[](صحيح i) ثابت
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع Drive_بټ();
			حالة DriveType::CONSTANT:
				أرجع constant_[i];
			حالة DriveType::WIRE:
				أرجع wire_[i];
			حالة DriveType::پورټ_2:
				أرجع پورټ_3[i];
			حالة DriveType::MARKER:
				أرجع marker_[i];
			حالة DriveType::مضرب:
				أرجع مضرب_3[i];
		}
		log_abort();
	}

	فراغ سېټ_none(صحيح پلنوالی = 0)
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				none_ = پلنوالی;
				اكسر;
			حالة DriveType::CONSTANT:
				constant_.~Const();
				اكسر;
			حالة DriveType::WIRE:
				wire_.~DriveChunkWire();
				اكسر;
			حالة DriveType::پورټ_2:
				پورټ_3.~Drive_Chunk_پورټ();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~DriveChunkMarker();
				اكسر;
			حالة DriveType::مضرب:
				مضرب_3.~Drive_Chunk_مضرب();
				اكسر;
		}
		type_ = DriveType::NONE;
		none_ = پلنوالی;
	}

	DriveChunk &چلوونکی=(DriveChunk ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سېټ_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::پورټ_2:
				*this = other.پورټ_3;
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

	DriveChunk &چلوونکی=(DriveChunk &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سېټ_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::پورټ_2:
				*this = std::move(other.پورټ_3);
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

	DriveChunk &چلوونکی=(Const ثابت &constant)
	{
		سېټ_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(Const &&constant)
	{
		سېټ_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(DriveChunkWire ثابت &wire)
	{
		سېټ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(DriveChunkWire &&wire)
	{
		سېټ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(Drive_Chunk_پورټ ثابت &پورټ)
	{
		سېټ_none();
		new (&پورټ_3) Drive_Chunk_پورټ(پورټ);
		type_ = DriveType::پورټ_2;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(Drive_Chunk_پورټ &&پورټ)
	{
		سېټ_none();
		new (&پورټ_3) Drive_Chunk_پورټ(پورټ);
		type_ = DriveType::پورټ_2;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(DriveChunkMarker ثابت &marker)
	{
		سېټ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(DriveChunkMarker &&marker)
	{
		سېټ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(Drive_Chunk_مضرب ثابت &مضرب_2)
	{
		سېټ_none(مضرب_2.اندازه());
		إذا (مضرب_2.مضرب_2().تشه())
			أرجع *this;
		new (&مضرب_3) Drive_Chunk_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(Drive_Chunk_مضرب &&مضرب_2)
	{
		سېټ_none(مضرب_2.اندازه());
		إذا (مضرب_2.مضرب_2().تشه())
			أرجع *this;
		new (&مضرب_3) Drive_Chunk_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	DriveChunk &چلوونکی=(Drive_بټ ثابت &other)
	{
		بدل (other.type())
		{
			حالة DriveType::NONE:
				سېټ_none(1);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = Const(other.constant());
				اكسر;
			حالة DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				اكسر;
			حالة DriveType::پورټ_2:
				*this = Drive_Chunk_پورټ(other.پورټ());
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

	bool can_append(Drive_بټ ثابت &بټ) ثابت;
	bool try_append(Drive_بټ ثابت &بټ);
	bool try_append(DriveChunk ثابت &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool چلوونکی==(ثابت DriveChunk &other) ثابت
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
			حالة DriveType::پورټ_2:
				أرجع پورټ_3 == other.پورټ_3;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::مضرب:
				أرجع مضرب_3 == other.مضرب_3;
		}
		log_abort();
	}

	bool چلوونکی!=(ثابت DriveChunk &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool چلوونکی<(ثابت DriveChunk &other) ثابت
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
			حالة DriveType::پورټ_2:
				أرجع پورټ_3 < other.پورټ_3;
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
	bool is_پورټ() ثابت { أرجع type_ == DriveType::پورټ_2; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_مضرب() ثابت { أرجع type_ == DriveType::مضرب; }

	Const &constant() { log_assert(is_constant()); أرجع constant_; }
	Const ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); أرجع wire_; }
	DriveChunkWire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	Drive_Chunk_پورټ &پورټ() { log_assert(is_پورټ()); أرجع پورټ_3; }
	Drive_Chunk_پورټ ثابت &پورټ() ثابت { log_assert(is_پورټ()); أرجع پورټ_3; }
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
			حالة DriveType::پورټ_2:
				أرجع پورټ_3.اندازه();
			حالة DriveType::MARKER:
				أرجع marker_.اندازه();
			حالة DriveType::مضرب:
				أرجع مضرب_3.اندازه();
		}
		log_abort();
	}
};

مضمن Drive_Chunk_مضرب::Drive_Chunk_مضرب(Drive_بټ_مضرب ثابت &بټ)
	: پلنوالی_3(1)
{
	لكل (خپلکاری ثابت &بټ : بټ.مضرب_2())
		مضرب_3.emplace(بټ);
}

بنية DriveSpec
{
خاص:
	صحيح پلنوالی_3 = 0;
	mutable std::ویکټور<DriveChunk> chunks_;
	mutable std::ویکټور<Drive_بټ> بټونه_2;
	mutable بینښه صحيح hash_ = 0;
عام:

	مضمن bool packed() ثابت {
		أرجع بټونه_2.تشه();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ثابت &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_پورټ ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ثابت &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_مضرب ثابت &chunk) { *this = chunk; }

	DriveSpec(Drive_بټ ثابت &بټ) { *this = بټ; }
	DriveSpec(Drive_بټ_Wire ثابت &بټ) { *this = بټ; }
	DriveSpec(Drive_بټ_پورټ ثابت &بټ) { *this = بټ; }
	DriveSpec(Drive_بټ_Marker ثابت &بټ) { *this = بټ; }
	DriveSpec(Drive_بټ_مضرب ثابت &بټ) { *this = بټ; }

	DriveSpec(std::ویکټور<DriveChunk> ثابت &chunks) : chunks_(chunks) { compute_پلنوالی(); }

	DriveSpec(std::ویکټور<Drive_بټ> ثابت &بټونه)
	{
		لكل (خپلکاری ثابت &بټ : بټونه)
			append(بټ);
	}

	DriveSpec(SigSpec ثابت &sig)
	{
		// TODO: converting one chunk at a time would be faster
		لكل (خپلکاری ثابت &بټ : sig.بټونه())
			append(بټ);
	}

	std::ویکټور<DriveChunk> ثابت &chunks() ثابت { pack(); أرجع chunks_; }
	std::ویکټور<Drive_بټ> ثابت &بټونه() ثابت { unpack(); أرجع بټونه_2; }

	صحيح اندازه() ثابت { أرجع پلنوالی_3; }

	فراغ append(Drive_بټ ثابت &بټ);

	فراغ append(DriveChunk ثابت &chunk);

	فراغ pack() ثابت;

	فراغ unpack() ثابت;

	Drive_بټ &چلوونکی[](صحيح شاخص)
	{
		log_assert(شاخص >= 0 && شاخص < اندازه());
		unpack();
		أرجع بټونه_2[شاخص];
	}

	ثابت Drive_بټ &چلوونکی[](صحيح شاخص) ثابت
	{
		log_assert(شاخص >= 0 && شاخص < اندازه());
		unpack();
		أرجع بټونه_2[شاخص];
	}

	فراغ clear()
	{
		chunks_.clear();
		بټونه_2.clear();
		پلنوالی_3 = 0;
	}

	DriveSpec &چلوونکی=(DriveChunk ثابت &chunk)
	{
		chunks_.clear();
		بټونه_2.clear();
		append(chunk);
		أرجع *this;
	}

	DriveSpec &چلوونکی=(DriveChunkWire ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &چلوونکی=(Drive_Chunk_پورټ ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &چلوونکی=(DriveChunkMarker ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &چلوونکی=(Drive_Chunk_مضرب ثابت &chunk) { أرجع *this = DriveChunk(chunk); }

	DriveSpec &چلوونکی=(Drive_بټ ثابت &بټ)
	{
		chunks_.clear();
		بټونه_2.clear();
		append(بټ);
		أرجع *this;
	}

	DriveSpec &چلوونکی=(Drive_بټ_Wire ثابت &بټ) { أرجع *this = Drive_بټ(بټ); }
	DriveSpec &چلوونکی=(Drive_بټ_پورټ ثابت &بټ) { أرجع *this = Drive_بټ(بټ); }
	DriveSpec &چلوونکی=(Drive_بټ_Marker ثابت &بټ) { أرجع *this = Drive_بټ(بټ); }
	DriveSpec &چلوونکی=(Drive_بټ_مضرب ثابت &بټ) { أرجع *this = Drive_بټ(بټ); }

	فراغ updhash() ثابت {
		إذا (hash_ != 0)
			أرجع;
		pack();
		hash_ = پرلپسې_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool چلوونکی==(DriveSpec ثابت &other) ثابت {
		updhash();
		other.updhash();
		إذا (اندازه() != other.اندازه() || hash_ != other.hash_)
			أرجع false;
		أرجع chunks() == other.chunks();
	}

خاص:
	فراغ compute_پلنوالی();
};



بنية DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.تیاری(); }
	DriverMap(Design *design) { celltypes.تیاری(design); }

خاص:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	بنية Drive_بټ_Id
	{
		صحيح id = -1;

		Drive_بټ_Id() {};

		Drive_بټ_Id(صحيح id) : id(id) { }

		bool چلوونکی==(ثابت Drive_بټ_Id &other) ثابت { أرجع id == other.id; }
		bool چلوونکی!=(ثابت Drive_بټ_Id &other) ثابت { أرجع id != other.id; }
		bool چلوونکی<(ثابت Drive_بټ_Id &other) ثابت { أرجع id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	بنية Drive_بټ_ګراف {
		dict<Drive_بټ_Id, Drive_بټ_Id> لومړی_څنډې;
		dict<Drive_بټ_Id, Drive_بټ_Id> دوهم_څنډې;
		dict<Drive_بټ_Id, pool<Drive_بټ_Id>> نور_څنډې;

		فراغ جمع_کړه_څنډه(Drive_بټ_Id src, Drive_بټ_Id dst);
		Drive_بټ_Id راوباسه_څنډه(Drive_بټ_Id src);
		فراغ clear(Drive_بټ_Id src);
		bool لري(Drive_بټ_Id src);
		صحيح شمېر(Drive_بټ_Id src);

		Drive_بټ_Id at(Drive_بټ_Id src, صحيح شاخص);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, Drive_بټ_Id> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, Drive_بټ_Id> پورټ_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<Drive_بټ_Id, Drive_بټ> drive_بټونه;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<Drive_بټ_Id, Drive_بټ> isolated_drive_بټونه;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	صحيح بل_دننه_واټن = 1 + (صحيح)State::Sm;

	// Union-Find over directly connected bits that share the same single
	// driver or are undriven. We never merge connections between drivers
	// and/or kept wires.
	mfp<Drive_بټ_Id> same_driver;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally unambiguous (one side only drives or requires a driven value).
	Drive_بټ_ګراف connected_drivers;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally ambiguous. Every such ambiguous connection is also present in
	// the reverse direction and has to be resolved when querying drivers.
	Drive_بټ_ګراف connected_undirected;

	// Subset of `connected_undirected` for caching the resolved driving
	// direction. In case multiple drivers are present this can still contain
	// both orientations of a single connection, but for a single driver only
	// one will be present.
	Drive_بټ_ګراف connected_oriented;

	// Stores for which bits we already resolved the orientation (cached in
	// `connected_oriented`).
	pool<Drive_بټ_Id> oriented_present;


	تعداد class بټ_ډېرپېښ {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	بټ_ډېرپېښ بټ_ډېرپېښ_2(Drive_بټ ثابت &بټ);
	Drive_بټ_Id id_from_drive_بټ(Drive_بټ ثابت &بټ);
	Drive_بټ drive_بټ_from_id(Drive_بټ_Id id);

	فراغ connect_directed_یوځای(Drive_بټ_Id driven_id, Drive_بټ_Id driver_id);
	فراغ connect_directed_بفر(Drive_بټ_Id driven_id, Drive_بټ_Id driver_id);
	فراغ connect_undirected(Drive_بټ_Id a_id, Drive_بټ_Id b_id);

عام:

	فراغ جمع_کړه(Module *module);

	// Add a single bit connection to the driver map.
	فراغ جمع_کړه(Drive_بټ ثابت &a, Drive_بټ ثابت &b);

	کينډۍ<typename T>
	ساكن constexpr bool is_sig_type() {
		أرجع
			std::is_same<T, SigSpec>::ارزښت ||
			std::is_same<T, SigChunk>::ارزښت ||
			std::is_same<T, DriveSpec>::ارزښت ||
			std::is_same<T, DriveChunk>::ارزښت ||
			std::is_same<T, Drive_Chunk_پورټ>::ارزښت ||
			std::is_same<T, DriveChunkWire>::ارزښت ||
			std::is_same<T, Const>::ارزښت;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	کينډۍ<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	جمع_کړه(T ثابت &a, U ثابت &b)
	{
		log_assert(a.اندازه() == b.اندازه());
		لكل (صحيح i = 0; i != Get_اندازه(a); ++i)
			جمع_کړه(Drive_بټ(a[i]), Drive_بټ(b[i]));
	}


	// Specialized version that avoids unpacking
	فراغ جمع_کړه(SigSpec ثابت &a, SigSpec ثابت &b);

خاص:
	فراغ جمع_کړه_پورټ(Cell *cell, IdString پورټ, SigSpec ثابت &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<Drive_بټ_Id> orient_undirected_کتل_شوی;
	pool<Drive_بټ_Id> orient_undirected_drivers;
	dict<Drive_بټ_Id, صحيح> orient_undirected_واټن;

	فراغ orient_undirected(Drive_بټ_Id id);

عام:
	Drive_بټ چلوونکی()(Drive_بټ ثابت &بټ);

	DriveSpec چلوونکی()(DriveSpec spec);

خاص:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		أرجع wire->has_attribute(ID(keep));
	}
};

مضمن Hasher Drive_بټ_Wire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(دننه_واټن);
	أرجع h;
}

مضمن Hasher Drive_بټ_پورټ::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورټ);
	h.eat(دننه_واټن);
	أرجع h;
}

مضمن Hasher Drive_بټ_Marker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(دننه_واټن);
	أرجع h;
}

مضمن Hasher Drive_بټ_مضرب::hash_into(Hasher h) ثابت
{
	h.eat(مضرب_3);
	أرجع h;
}

مضمن Hasher Drive_بټ::hash_into(Hasher h) ثابت
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
	حالة DriveType::پورټ_2:
		h.eat(پورټ_3);
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
	h.eat(پلنوالی);
	h.eat(دننه_واټن);
	أرجع h;
}

مضمن Hasher Drive_Chunk_پورټ::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورټ);
	h.eat(پلنوالی);
	h.eat(دننه_واټن);
	أرجع h;
}

مضمن Hasher DriveChunkMarker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(پلنوالی);
	h.eat(دننه_واټن);
	أرجع h;
}

مضمن Hasher Drive_Chunk_مضرب::hash_into(Hasher h) ثابت
{
	h.eat(پلنوالی_3);
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
	حالة DriveType::پورټ_2:
		h.eat(پورټ_3);
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

مضمن Hasher DriverMap::Drive_بټ_Id::hash_into(Hasher h) ثابت
{
	h.eat(id);
	أرجع h;
}

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
