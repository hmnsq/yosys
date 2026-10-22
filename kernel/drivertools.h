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

#جيڪڏهن_وصف_نه_ٿيل DRIVERTOOLS_H
#وصف_ڪريو DRIVERTOOLS_H

#ضمن <string>
#ضمن <type_traits>

#ضمن "kernel/rtlil.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

بنية Drive_ٻٽو_انگ;

بنية DriveChunkWire;
بنية Drive_Chunk_پورٽ;
بنية DriveChunkMarker;
بنية DriveChunk;

بنية DriveSpec;

std::string log_signal(DriveChunkWire ثابت &chunk);
std::string log_signal(Drive_Chunk_پورٽ ثابت &chunk);
std::string log_signal(DriveChunkMarker ثابت &chunk);
std::string log_signal(DriveChunk ثابت &chunk);
std::string log_signal(DriveSpec ثابت &chunk);

تعداد class DriveType : بنانشان حرف
{
	NONE,
	CONSTANT,
	WIRE,
	پورٽ_2,
	گڻو,
	MARKER,
};

بنية Drive_ٻٽو_انگ_Wire
{
	Wire *wire;
	صحيح وٿي;

	Drive_ٻٽو_انگ_Wire(Wire *wire, صحيح وٿي) : wire(wire), وٿي(وٿي) {}

	bool عامل==(ثابت Drive_ٻٽو_انگ_Wire &other) ثابت
	{
		أرجع wire == other.wire && وٿي == other.وٿي;
	}

	bool عامل<(ثابت Drive_ٻٽو_انگ_Wire &other) ثابت
	{
		إذا (wire != other.wire)
			أرجع wire->name < other.wire->name;
		أرجع وٿي < other.وٿي;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;


	عامل Sig_ٻٽو_انگ() ثابت
	{
		أرجع Sig_ٻٽو_انگ(wire, وٿي);
	}
};

بنية Drive_ٻٽو_انگ_پورٽ
{
	Cell *cell;
	IdString پورٽ;
	صحيح وٿي;

	Drive_ٻٽو_انگ_پورٽ(Cell *cell, IdString پورٽ, صحيح وٿي) : cell(cell), پورٽ(پورٽ), وٿي(وٿي) {}

	bool عامل==(ثابت Drive_ٻٽو_انگ_پورٽ &other) ثابت
	{
		أرجع cell == other.cell && پورٽ == other.پورٽ && وٿي == other.وٿي;
	}

	bool عامل<(ثابت Drive_ٻٽو_انگ_پورٽ &other) ثابت
	{
		إذا (cell != other.cell)
			أرجع cell->name < other.cell->name;
		إذا (پورٽ != other.پورٽ)
			أرجع پورٽ < other.پورٽ;
		أرجع وٿي < other.وٿي;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};


بنية Drive_ٻٽو_انگ_Marker
{
	صحيح marker;
	صحيح وٿي;

	Drive_ٻٽو_انگ_Marker(صحيح marker, صحيح وٿي) : marker(marker), وٿي(وٿي) {}

	bool عامل==(ثابت Drive_ٻٽو_انگ_Marker &other) ثابت
	{
		أرجع marker == other.marker && وٿي == other.وٿي;
	}

	bool عامل<(ثابت Drive_ٻٽو_انگ_Marker &other) ثابت
	{
		إذا (marker != other.marker)
			أرجع marker < other.marker;
		أرجع وٿي < other.وٿي;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};

بنية Drive_ٻٽو_انگ_گڻو
{
خاص:
	pool<Drive_ٻٽو_انگ> گڻو_3;

عام:
	Drive_ٻٽو_انگ_گڻو();
	Drive_ٻٽو_انگ_گڻو(Drive_ٻٽو_انگ ثابت &single);

	pool<Drive_ٻٽو_انگ> ثابت &گڻو_2() ثابت { أرجع گڻو_3; }

	فراغ ملايو(Drive_ٻٽو_انگ_گڻو ثابت &other)
	{
		لكل (Drive_ٻٽو_انگ ثابت &single : other.گڻو_3)
			ملايو(single);
	}

	فراغ ملايو(Drive_ٻٽو_انگ_گڻو &&other)
	{
		لكل (Drive_ٻٽو_انگ &single : other.گڻو_3)
			ملايو(std::move(single));
	}

	فراغ ملايو(Drive_ٻٽو_انگ ثابت &single);
	فراغ ملايو(Drive_ٻٽو_انگ &&single);

	bool عامل==(ثابت Drive_ٻٽو_انگ_گڻو &other) ثابت
	{
		أرجع گڻو_3 == other.گڻو_3;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Drive_ٻٽو_انگ
{
خاص:
	DriveType type_ = DriveType::NONE;
	اتحاد
	{
		State constant_;
		Drive_ٻٽو_انگ_Wire wire_;
		Drive_ٻٽو_انگ_پورٽ پورٽ_3;
		Drive_ٻٽو_انگ_Marker marker_;
		Drive_ٻٽو_انگ_گڻو گڻو_3;
	};
عام:
	Drive_ٻٽو_انگ()  {}

	Drive_ٻٽو_انگ(Sig_ٻٽو_انگ ثابت &ٻٽو_انگ);

	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ ثابت &other) { *this = other; }
	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ &&other) { *this = other; }


	Drive_ٻٽو_انگ(State constant) { *this = constant; }
	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ_Wire ثابت &wire) { *this = wire; }
	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ_Wire &&wire) { *this = wire; }
	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ_پورٽ ثابت &پورٽ) { *this = پورٽ; }
	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ_پورٽ &&پورٽ) { *this = پورٽ; }
	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ_Marker ثابت &marker) { *this = marker; }
	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ_Marker &&marker) { *this = marker; }
	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ_گڻو ثابت &گڻو_2) { *this = گڻو_2; }
	Drive_ٻٽو_انگ(Drive_ٻٽو_انگ_گڻو &&گڻو_2) { *this = گڻو_2; }

	~Drive_ٻٽو_انگ() { سيٽ_none(); }

	فراغ سيٽ_none()
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				اكسر;
			حالة DriveType::CONSTANT:
				اكسر;
			حالة DriveType::WIRE:
				wire_.~Drive_ٻٽو_انگ_Wire();
				اكسر;
			حالة DriveType::پورٽ_2:
				پورٽ_3.~Drive_ٻٽو_انگ_پورٽ();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~Drive_ٻٽو_انگ_Marker();
				اكسر;
			حالة DriveType::گڻو:
				گڻو_3.~Drive_ٻٽو_انگ_گڻو();
				اكسر;
		}
		type_ = DriveType::NONE;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سيٽ_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::پورٽ_2:
				*this = other.پورٽ_3;
				اكسر;
			حالة DriveType::MARKER:
				*this = other.marker_;
				اكسر;
			حالة DriveType::گڻو:
				*this = other.گڻو_3;
				اكسر;
		}
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سيٽ_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::پورٽ_2:
				*this = std::move(other.پورٽ_3);
				اكسر;
			حالة DriveType::MARKER:
				*this = std::move(other.marker_);
				اكسر;
			حالة DriveType::گڻو:
				*this = std::move(other.گڻو_3);
				اكسر;
		}
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(State constant)
	{
		سيٽ_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ_Wire ثابت &wire)
	{
		سيٽ_none();
		new (&wire_) Drive_ٻٽو_انگ_Wire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ_Wire &&wire)
	{
		سيٽ_none();
		new (&wire_) Drive_ٻٽو_انگ_Wire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ_پورٽ ثابت &پورٽ)
	{
		سيٽ_none();
		new (&پورٽ_3) Drive_ٻٽو_انگ_پورٽ(پورٽ);
		type_ = DriveType::پورٽ_2;
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ_پورٽ &&پورٽ)
	{
		سيٽ_none();
		new (&پورٽ_3) Drive_ٻٽو_انگ_پورٽ(پورٽ);
		type_ = DriveType::پورٽ_2;
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ_Marker ثابت &marker)
	{
		سيٽ_none();
		new (&marker_) Drive_ٻٽو_انگ_Marker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ_Marker &&marker)
	{
		سيٽ_none();
		new (&marker_) Drive_ٻٽو_انگ_Marker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ_گڻو ثابت &گڻو_2)
	{
		سيٽ_none();
		إذا (گڻو_2.گڻو_2().خالي_جايون())
			أرجع *this;
		new (&گڻو_3) Drive_ٻٽو_انگ_گڻو(گڻو_2);
		type_ = DriveType::گڻو;
		أرجع *this;
	}

	Drive_ٻٽو_انگ &عامل=(Drive_ٻٽو_انگ_گڻو &&گڻو_2)
	{
		سيٽ_none();
		إذا (گڻو_2.گڻو_2().خالي_جايون())
			أرجع *this;
		new (&گڻو_3) Drive_ٻٽو_انگ_گڻو(گڻو_2);
		type_ = DriveType::گڻو;
		أرجع *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عامل==(ثابت Drive_ٻٽو_انگ &other) ثابت
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
			حالة DriveType::پورٽ_2:
				أرجع پورٽ_3 == other.پورٽ_3;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::گڻو:
				أرجع گڻو_3 == other.گڻو_3;
		}
		log_abort();
	}

	bool عامل!=(ثابت Drive_ٻٽو_انگ &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool عامل<(ثابت Drive_ٻٽو_انگ &other) ثابت
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
			حالة DriveType::پورٽ_2:
				أرجع پورٽ_3 < other.پورٽ_3;
			حالة DriveType::MARKER:
				أرجع marker_ < other.marker_;
			حالة DriveType::گڻو:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ثابت { أرجع type_; }

	bool is_none() ثابت { أرجع type_ == DriveType::NONE; }
	bool is_constant() ثابت { أرجع type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { أرجع type_ == DriveType::WIRE; }
	bool is_پورٽ() ثابت { أرجع type_ == DriveType::پورٽ_2; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_گڻو() ثابت { أرجع type_ == DriveType::گڻو; }

	State &constant() { log_assert(is_constant()); أرجع constant_; }
	State ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	Drive_ٻٽو_انگ_Wire &wire() { log_assert(is_wire()); أرجع wire_; }
	Drive_ٻٽو_انگ_Wire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	Drive_ٻٽو_انگ_پورٽ &پورٽ() { log_assert(is_پورٽ()); أرجع پورٽ_3; }
	Drive_ٻٽو_انگ_پورٽ ثابت &پورٽ() ثابت { log_assert(is_پورٽ()); أرجع پورٽ_3; }
	Drive_ٻٽو_انگ_Marker &marker() { log_assert(is_marker()); أرجع marker_; }
	Drive_ٻٽو_انگ_Marker ثابت &marker() ثابت { log_assert(is_marker()); أرجع marker_; }
	Drive_ٻٽو_انگ_گڻو &گڻو_2() { log_assert(is_گڻو()); أرجع گڻو_3; }
	Drive_ٻٽو_انگ_گڻو ثابت &گڻو_2() ثابت { log_assert(is_گڻو()); أرجع گڻو_3; }

	فراغ ملايو(Drive_ٻٽو_انگ ثابت &other);

};

مضمن Drive_ٻٽو_انگ_گڻو::Drive_ٻٽو_انگ_گڻو() {}
مضمن Drive_ٻٽو_انگ_گڻو::Drive_ٻٽو_انگ_گڻو(Drive_ٻٽو_انگ ثابت &single)
{
	گڻو_3.emplace(single);
}

بنية DriveChunkWire
{
	Wire *wire;
	صحيح وٿي;
	صحيح ويڪر;

	DriveChunkWire(Wire *wire, صحيح وٿي, صحيح ويڪر) : wire(wire), وٿي(وٿي), ويڪر(ويڪر) {}
	DriveChunkWire(Drive_ٻٽو_انگ_Wire ثابت &ٻٽو_انگ) : wire(ٻٽو_انگ.wire), وٿي(ٻٽو_انگ.وٿي), ويڪر(1) {}

	صحيح سائيز() ثابت { أرجع ويڪر; }

	Drive_ٻٽو_انگ_Wire عامل[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < ويڪر);
		أرجع Drive_ٻٽو_انگ_Wire(wire, وٿي + i);
	}

	bool can_append(Drive_ٻٽو_انگ_Wire ثابت &ٻٽو_انگ) ثابت;
	bool try_append(Drive_ٻٽو_انگ_Wire ثابت &ٻٽو_انگ);
	bool try_append(DriveChunkWire ثابت &chunk);

	// Whether this chunk is a whole wire
	bool is_سڄو() ثابت { أرجع وٿي == 0 && ويڪر == wire->ويڪر; }

	bool عامل==(ثابت DriveChunkWire &other) ثابت
	{
		أرجع wire == other.wire && وٿي == other.وٿي && ويڪر == other.ويڪر;
	}

	bool عامل<(ثابت DriveChunkWire &other) ثابت
	{
		إذا (wire != other.wire)
			أرجع wire->name < other.wire->name;
		إذا (ويڪر != other.ويڪر)
			أرجع ويڪر < other.ويڪر;
		أرجع وٿي < other.وٿي;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	explicit عامل SigChunk() ثابت
	{
		أرجع SigChunk(wire, وٿي, ويڪر);
	}
};

بنية Drive_Chunk_پورٽ
{
	Cell *cell;
	IdString پورٽ;
	صحيح وٿي;
	صحيح ويڪر;

	Drive_Chunk_پورٽ(Cell *cell, IdString پورٽ, صحيح وٿي, صحيح ويڪر) :
		cell(cell), پورٽ(پورٽ), وٿي(وٿي), ويڪر(ويڪر) { }
	Drive_Chunk_پورٽ(Cell *cell, IdString پورٽ) :
		cell(cell), پورٽ(پورٽ), وٿي(0), ويڪر(Get_سائيز(cell->connections().at(پورٽ))) { }
	Drive_Chunk_پورٽ(Cell *cell, std::pair<IdString, SigSpec> ثابت &conn) :
		cell(cell), پورٽ(conn.پهريون), وٿي(0), ويڪر(Get_سائيز(conn.ٻيو)) { }
	Drive_Chunk_پورٽ(Drive_ٻٽو_انگ_پورٽ ثابت &ٻٽو_انگ) :
		cell(ٻٽو_انگ.cell), پورٽ(ٻٽو_انگ.پورٽ), وٿي(ٻٽو_انگ.وٿي), ويڪر(1) { }

	صحيح سائيز() ثابت { أرجع ويڪر; }

	Drive_ٻٽو_انگ_پورٽ عامل[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < ويڪر);
		أرجع Drive_ٻٽو_انگ_پورٽ(cell, پورٽ, وٿي + i);
	}

	bool can_append(Drive_ٻٽو_انگ_پورٽ ثابت &ٻٽو_انگ) ثابت;
	bool try_append(Drive_ٻٽو_انگ_پورٽ ثابت &ٻٽو_انگ);
	bool try_append(Drive_Chunk_پورٽ ثابت &chunk);

	// Whether this chunk is a whole port
	bool is_سڄو() ثابت
	{
		أرجع وٿي == 0 && ويڪر == cell->connections().at(پورٽ).سائيز();
	}

	bool عامل==(ثابت Drive_Chunk_پورٽ &other) ثابت
	{
		أرجع cell == other.cell && پورٽ == other.پورٽ && وٿي == other.وٿي && ويڪر == other.ويڪر;
	}

	bool عامل<(ثابت Drive_Chunk_پورٽ &other) ثابت
	{
		إذا (cell != other.cell)
			أرجع cell->name < other.cell->name;
		إذا (پورٽ != other.پورٽ)
			أرجع پورٽ < other.پورٽ;
		إذا (ويڪر != other.ويڪر)
			أرجع ويڪر < other.ويڪر;
		أرجع وٿي < other.وٿي;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};


بنية DriveChunkMarker
{
	صحيح marker;
	صحيح وٿي;
	صحيح ويڪر;

	DriveChunkMarker(صحيح marker, صحيح وٿي, صحيح ويڪر) :
		marker(marker), وٿي(وٿي), ويڪر(ويڪر) {}
	DriveChunkMarker(Drive_ٻٽو_انگ_Marker ثابت &ٻٽو_انگ) :
		marker(ٻٽو_انگ.marker), وٿي(ٻٽو_انگ.وٿي), ويڪر(1) {}

	صحيح سائيز() ثابت { أرجع ويڪر; }

	Drive_ٻٽو_انگ_Marker عامل[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < ويڪر);
		أرجع Drive_ٻٽو_انگ_Marker(marker, وٿي + i);
	}

	bool can_append(Drive_ٻٽو_انگ_Marker ثابت &ٻٽو_انگ) ثابت;
	bool try_append(Drive_ٻٽو_انگ_Marker ثابت &ٻٽو_انگ);
	bool try_append(DriveChunkMarker ثابت &chunk);

	bool عامل==(ثابت DriveChunkMarker &other) ثابت
	{
		أرجع marker == other.marker && وٿي == other.وٿي && ويڪر == other.ويڪر;
	}

	bool عامل<(ثابت DriveChunkMarker &other) ثابت
	{
		إذا (marker != other.marker)
			أرجع marker < other.marker;
		إذا (ويڪر != other.ويڪر)
			أرجع ويڪر < other.ويڪر;
		أرجع وٿي < other.وٿي;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Drive_Chunk_گڻو
{
خاص:
	mutable pool<DriveChunk> گڻو_3;
	صحيح ويڪر_3;

عام:
	pool<DriveChunk> ثابت &گڻو_2() ثابت { أرجع گڻو_3; }

	Drive_Chunk_گڻو(Drive_ٻٽو_انگ_گڻو ثابت &ٻٽو_انگ);

	صحيح سائيز() ثابت { أرجع ويڪر_3; }

	Drive_ٻٽو_انگ_گڻو عامل[](صحيح i) ثابت;

	bool can_append(Drive_ٻٽو_انگ_گڻو ثابت &ٻٽو_انگ) ثابت;

	bool try_append(Drive_ٻٽو_انگ_گڻو ثابت &ٻٽو_انگ);


	bool can_append(Drive_Chunk_گڻو ثابت &ٻٽو_انگ) ثابت;

	bool try_append(Drive_Chunk_گڻو ثابت &ٻٽو_انگ);

	bool عامل==(ثابت Drive_Chunk_گڻو &other) ثابت
	{
		أرجع ويڪر_3 == other.ويڪر_3 && گڻو_3 == other.گڻو_3;
	}

	bool عامل<(ثابت Drive_Chunk_گڻو &other) ثابت
	{
		إذا (گڻو_3.سائيز() < other.گڻو_3.سائيز())

		گڻو_3.ترتيب_ڏيو();
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
		Drive_Chunk_پورٽ پورٽ_3;
		DriveChunkMarker marker_;
		Drive_Chunk_گڻو گڻو_3;
	};

عام:
	DriveChunk() { سيٽ_none(); }

	DriveChunk(DriveChunk ثابت &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(Drive_ٻٽو_انگ ثابت &other) { *this = other; }

	DriveChunk(Const ثابت &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ثابت &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(Drive_Chunk_پورٽ ثابت &پورٽ) { *this = پورٽ; }
	DriveChunk(Drive_Chunk_پورٽ &&پورٽ) { *this = پورٽ; }
	DriveChunk(DriveChunkMarker ثابت &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(Drive_Chunk_گڻو ثابت &گڻو_2) { *this = گڻو_2; }
	DriveChunk(Drive_Chunk_گڻو &&گڻو_2) { *this = گڻو_2; }

	~DriveChunk() { سيٽ_none(); }

	Drive_ٻٽو_انگ عامل[](صحيح i) ثابت
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع Drive_ٻٽو_انگ();
			حالة DriveType::CONSTANT:
				أرجع constant_[i];
			حالة DriveType::WIRE:
				أرجع wire_[i];
			حالة DriveType::پورٽ_2:
				أرجع پورٽ_3[i];
			حالة DriveType::MARKER:
				أرجع marker_[i];
			حالة DriveType::گڻو:
				أرجع گڻو_3[i];
		}
		log_abort();
	}

	فراغ سيٽ_none(صحيح ويڪر = 0)
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				none_ = ويڪر;
				اكسر;
			حالة DriveType::CONSTANT:
				constant_.~Const();
				اكسر;
			حالة DriveType::WIRE:
				wire_.~DriveChunkWire();
				اكسر;
			حالة DriveType::پورٽ_2:
				پورٽ_3.~Drive_Chunk_پورٽ();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~DriveChunkMarker();
				اكسر;
			حالة DriveType::گڻو:
				گڻو_3.~Drive_Chunk_گڻو();
				اكسر;
		}
		type_ = DriveType::NONE;
		none_ = ويڪر;
	}

	DriveChunk &عامل=(DriveChunk ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سيٽ_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::پورٽ_2:
				*this = other.پورٽ_3;
				اكسر;
			حالة DriveType::MARKER:
				*this = other.marker_;
				اكسر;
			حالة DriveType::گڻو:
				*this = other.گڻو_3;
				اكسر;
		}
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunk &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سيٽ_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::پورٽ_2:
				*this = std::move(other.پورٽ_3);
				اكسر;
			حالة DriveType::MARKER:
				*this = std::move(other.marker_);
				اكسر;
			حالة DriveType::گڻو:
				*this = std::move(other.گڻو_3);
				اكسر;
		}
		أرجع *this;
	}

	DriveChunk &عامل=(Const ثابت &constant)
	{
		سيٽ_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &عامل=(Const &&constant)
	{
		سيٽ_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunkWire ثابت &wire)
	{
		سيٽ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunkWire &&wire)
	{
		سيٽ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_Chunk_پورٽ ثابت &پورٽ)
	{
		سيٽ_none();
		new (&پورٽ_3) Drive_Chunk_پورٽ(پورٽ);
		type_ = DriveType::پورٽ_2;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_Chunk_پورٽ &&پورٽ)
	{
		سيٽ_none();
		new (&پورٽ_3) Drive_Chunk_پورٽ(پورٽ);
		type_ = DriveType::پورٽ_2;
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunkMarker ثابت &marker)
	{
		سيٽ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunkMarker &&marker)
	{
		سيٽ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_Chunk_گڻو ثابت &گڻو_2)
	{
		سيٽ_none(گڻو_2.سائيز());
		إذا (گڻو_2.گڻو_2().خالي_جايون())
			أرجع *this;
		new (&گڻو_3) Drive_Chunk_گڻو(گڻو_2);
		type_ = DriveType::گڻو;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_Chunk_گڻو &&گڻو_2)
	{
		سيٽ_none(گڻو_2.سائيز());
		إذا (گڻو_2.گڻو_2().خالي_جايون())
			أرجع *this;
		new (&گڻو_3) Drive_Chunk_گڻو(گڻو_2);
		type_ = DriveType::گڻو;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_ٻٽو_انگ ثابت &other)
	{
		بدل (other.type())
		{
			حالة DriveType::NONE:
				سيٽ_none(1);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = Const(other.constant());
				اكسر;
			حالة DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				اكسر;
			حالة DriveType::پورٽ_2:
				*this = Drive_Chunk_پورٽ(other.پورٽ());
				اكسر;
			حالة DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				اكسر;
			حالة DriveType::گڻو:
				*this = Drive_Chunk_گڻو(other.گڻو_2());
				اكسر;
		}
		أرجع *this;
	}

	bool can_append(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ) ثابت;
	bool try_append(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ);
	bool try_append(DriveChunk ثابت &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عامل==(ثابت DriveChunk &other) ثابت
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
			حالة DriveType::پورٽ_2:
				أرجع پورٽ_3 == other.پورٽ_3;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::گڻو:
				أرجع گڻو_3 == other.گڻو_3;
		}
		log_abort();
	}

	bool عامل!=(ثابت DriveChunk &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool عامل<(ثابت DriveChunk &other) ثابت
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
			حالة DriveType::پورٽ_2:
				أرجع پورٽ_3 < other.پورٽ_3;
			حالة DriveType::MARKER:
				أرجع marker_ < other.marker_;
			حالة DriveType::گڻو:
				أرجع گڻو_3 < other.گڻو_3;
		}
		log_abort();
	}

	DriveType type() ثابت { أرجع type_; }

	bool is_none() ثابت { أرجع type_ == DriveType::NONE; }
	bool is_constant() ثابت { أرجع type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { أرجع type_ == DriveType::WIRE; }
	bool is_پورٽ() ثابت { أرجع type_ == DriveType::پورٽ_2; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_گڻو() ثابت { أرجع type_ == DriveType::گڻو; }

	Const &constant() { log_assert(is_constant()); أرجع constant_; }
	Const ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); أرجع wire_; }
	DriveChunkWire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	Drive_Chunk_پورٽ &پورٽ() { log_assert(is_پورٽ()); أرجع پورٽ_3; }
	Drive_Chunk_پورٽ ثابت &پورٽ() ثابت { log_assert(is_پورٽ()); أرجع پورٽ_3; }
	DriveChunkMarker &marker() { log_assert(is_marker()); أرجع marker_; }
	DriveChunkMarker ثابت &marker() ثابت { log_assert(is_marker()); أرجع marker_; }
	Drive_Chunk_گڻو &گڻو_2() { log_assert(is_گڻو()); أرجع گڻو_3; }
	Drive_Chunk_گڻو ثابت &گڻو_2() ثابت { log_assert(is_گڻو()); أرجع گڻو_3; }


	صحيح سائيز() ثابت
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع none_;
			حالة DriveType::CONSTANT:
				أرجع constant_.سائيز();
			حالة DriveType::WIRE:
				أرجع wire_.سائيز();
			حالة DriveType::پورٽ_2:
				أرجع پورٽ_3.سائيز();
			حالة DriveType::MARKER:
				أرجع marker_.سائيز();
			حالة DriveType::گڻو:
				أرجع گڻو_3.سائيز();
		}
		log_abort();
	}
};

مضمن Drive_Chunk_گڻو::Drive_Chunk_گڻو(Drive_ٻٽو_انگ_گڻو ثابت &ٻٽو_انگ)
	: ويڪر_3(1)
{
	لكل (خودڪار ثابت &ٻٽو_انگ : ٻٽو_انگ.گڻو_2())
		گڻو_3.emplace(ٻٽو_انگ);
}

بنية DriveSpec
{
خاص:
	صحيح ويڪر_3 = 0;
	mutable std::ويڪٽر<DriveChunk> chunks_;
	mutable std::ويڪٽر<Drive_ٻٽو_انگ> بٽ_2;
	mutable بنانشان صحيح hash_ = 0;
عام:

	مضمن bool packed() ثابت {
		أرجع بٽ_2.خالي_جايون();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ثابت &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_پورٽ ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ثابت &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_گڻو ثابت &chunk) { *this = chunk; }

	DriveSpec(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ) { *this = ٻٽو_انگ; }
	DriveSpec(Drive_ٻٽو_انگ_Wire ثابت &ٻٽو_انگ) { *this = ٻٽو_انگ; }
	DriveSpec(Drive_ٻٽو_انگ_پورٽ ثابت &ٻٽو_انگ) { *this = ٻٽو_انگ; }
	DriveSpec(Drive_ٻٽو_انگ_Marker ثابت &ٻٽو_انگ) { *this = ٻٽو_انگ; }
	DriveSpec(Drive_ٻٽو_انگ_گڻو ثابت &ٻٽو_انگ) { *this = ٻٽو_انگ; }

	DriveSpec(std::ويڪٽر<DriveChunk> ثابت &chunks) : chunks_(chunks) { compute_ويڪر(); }

	DriveSpec(std::ويڪٽر<Drive_ٻٽو_انگ> ثابت &بٽ)
	{
		لكل (خودڪار ثابت &ٻٽو_انگ : بٽ)
			append(ٻٽو_انگ);
	}

	DriveSpec(SigSpec ثابت &sig)
	{
		// TODO: converting one chunk at a time would be faster
		لكل (خودڪار ثابت &ٻٽو_انگ : sig.بٽ())
			append(ٻٽو_انگ);
	}

	std::ويڪٽر<DriveChunk> ثابت &chunks() ثابت { pack(); أرجع chunks_; }
	std::ويڪٽر<Drive_ٻٽو_انگ> ثابت &بٽ() ثابت { unpack(); أرجع بٽ_2; }

	صحيح سائيز() ثابت { أرجع ويڪر_3; }

	فراغ append(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ);

	فراغ append(DriveChunk ثابت &chunk);

	فراغ pack() ثابت;

	فراغ unpack() ثابت;

	Drive_ٻٽو_انگ &عامل[](صحيح اشاريو)
	{
		log_assert(اشاريو >= 0 && اشاريو < سائيز());
		unpack();
		أرجع بٽ_2[اشاريو];
	}

	ثابت Drive_ٻٽو_انگ &عامل[](صحيح اشاريو) ثابت
	{
		log_assert(اشاريو >= 0 && اشاريو < سائيز());
		unpack();
		أرجع بٽ_2[اشاريو];
	}

	فراغ clear()
	{
		chunks_.clear();
		بٽ_2.clear();
		ويڪر_3 = 0;
	}

	DriveSpec &عامل=(DriveChunk ثابت &chunk)
	{
		chunks_.clear();
		بٽ_2.clear();
		append(chunk);
		أرجع *this;
	}

	DriveSpec &عامل=(DriveChunkWire ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &عامل=(Drive_Chunk_پورٽ ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &عامل=(DriveChunkMarker ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &عامل=(Drive_Chunk_گڻو ثابت &chunk) { أرجع *this = DriveChunk(chunk); }

	DriveSpec &عامل=(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ)
	{
		chunks_.clear();
		بٽ_2.clear();
		append(ٻٽو_انگ);
		أرجع *this;
	}

	DriveSpec &عامل=(Drive_ٻٽو_انگ_Wire ثابت &ٻٽو_انگ) { أرجع *this = Drive_ٻٽو_انگ(ٻٽو_انگ); }
	DriveSpec &عامل=(Drive_ٻٽو_انگ_پورٽ ثابت &ٻٽو_انگ) { أرجع *this = Drive_ٻٽو_انگ(ٻٽو_انگ); }
	DriveSpec &عامل=(Drive_ٻٽو_انگ_Marker ثابت &ٻٽو_انگ) { أرجع *this = Drive_ٻٽو_انگ(ٻٽو_انگ); }
	DriveSpec &عامل=(Drive_ٻٽو_انگ_گڻو ثابت &ٻٽو_انگ) { أرجع *this = Drive_ٻٽو_انگ(ٻٽو_انگ); }

	فراغ updhash() ثابت {
		إذا (hash_ != 0)
			أرجع;
		pack();
		hash_ = لاڳيتو_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عامل==(DriveSpec ثابت &other) ثابت {
		updhash();
		other.updhash();
		إذا (سائيز() != other.سائيز() || hash_ != other.hash_)
			أرجع false;
		أرجع chunks() == other.chunks();
	}

خاص:
	فراغ compute_ويڪر();
};



بنية DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.تياري(); }
	DriverMap(Design *design) { celltypes.تياري(design); }

خاص:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	بنية Drive_ٻٽو_انگ_Id
	{
		صحيح id = -1;

		Drive_ٻٽو_انگ_Id() {};

		Drive_ٻٽو_انگ_Id(صحيح id) : id(id) { }

		bool عامل==(ثابت Drive_ٻٽو_انگ_Id &other) ثابت { أرجع id == other.id; }
		bool عامل!=(ثابت Drive_ٻٽو_انگ_Id &other) ثابت { أرجع id != other.id; }
		bool عامل<(ثابت Drive_ٻٽو_انگ_Id &other) ثابت { أرجع id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	بنية Drive_ٻٽو_انگ_گراف {
		dict<Drive_ٻٽو_انگ_Id, Drive_ٻٽو_انگ_Id> پهريون_ڪنارا;
		dict<Drive_ٻٽو_انگ_Id, Drive_ٻٽو_انگ_Id> ٻيو_ڪنارا;
		dict<Drive_ٻٽو_انگ_Id, pool<Drive_ٻٽو_انگ_Id>> وڌيڪ_ڪنارا;

		فراغ جوڙيو_ڪنارو(Drive_ٻٽو_انگ_Id src, Drive_ٻٽو_انگ_Id dst);
		Drive_ٻٽو_انگ_Id ڪڍو_ڪنارو(Drive_ٻٽو_انگ_Id src);
		فراغ clear(Drive_ٻٽو_انگ_Id src);
		bool شامل_آهي(Drive_ٻٽو_انگ_Id src);
		صحيح ڳڻپ(Drive_ٻٽو_انگ_Id src);

		Drive_ٻٽو_انگ_Id at(Drive_ٻٽو_انگ_Id src, صحيح اشاريو);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, Drive_ٻٽو_انگ_Id> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, Drive_ٻٽو_انگ_Id> پورٽ_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<Drive_ٻٽو_انگ_Id, Drive_ٻٽو_انگ> drive_بٽ;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<Drive_ٻٽو_انگ_Id, Drive_ٻٽو_انگ> isolated_drive_بٽ;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	صحيح ايندڙ_وٿي = 1 + (صحيح)State::Sm;

	// Union-Find over directly connected bits that share the same single
	// driver or are undriven. We never merge connections between drivers
	// and/or kept wires.
	mfp<Drive_ٻٽو_انگ_Id> same_driver;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally unambiguous (one side only drives or requires a driven value).
	Drive_ٻٽو_انگ_گراف connected_drivers;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally ambiguous. Every such ambiguous connection is also present in
	// the reverse direction and has to be resolved when querying drivers.
	Drive_ٻٽو_انگ_گراف connected_undirected;

	// Subset of `connected_undirected` for caching the resolved driving
	// direction. In case multiple drivers are present this can still contain
	// both orientations of a single connection, but for a single driver only
	// one will be present.
	Drive_ٻٽو_انگ_گراف connected_oriented;

	// Stores for which bits we already resolved the orientation (cached in
	// `connected_oriented`).
	pool<Drive_ٻٽو_انگ_Id> oriented_present;


	تعداد class ٻٽو_انگ_گهڻائي {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	ٻٽو_انگ_گهڻائي ٻٽو_انگ_گهڻائي_2(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ);
	Drive_ٻٽو_انگ_Id id_from_drive_ٻٽو_انگ(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ);
	Drive_ٻٽو_انگ drive_ٻٽو_انگ_from_id(Drive_ٻٽو_انگ_Id id);

	فراغ connect_directed_ملايو(Drive_ٻٽو_انگ_Id driven_id, Drive_ٻٽو_انگ_Id driver_id);
	فراغ connect_directed_بفر(Drive_ٻٽو_انگ_Id driven_id, Drive_ٻٽو_انگ_Id driver_id);
	فراغ connect_undirected(Drive_ٻٽو_انگ_Id a_id, Drive_ٻٽو_انگ_Id b_id);

عام:

	فراغ جوڙيو(Module *module);

	// Add a single bit connection to the driver map.
	فراغ جوڙيو(Drive_ٻٽو_انگ ثابت &a, Drive_ٻٽو_انگ ثابت &b);

	سانچو<typename T>
	ساكن constexpr bool is_sig_type() {
		أرجع
			std::is_same<T, SigSpec>::قدر ||
			std::is_same<T, SigChunk>::قدر ||
			std::is_same<T, DriveSpec>::قدر ||
			std::is_same<T, DriveChunk>::قدر ||
			std::is_same<T, Drive_Chunk_پورٽ>::قدر ||
			std::is_same<T, DriveChunkWire>::قدر ||
			std::is_same<T, Const>::قدر;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	سانچو<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	جوڙيو(T ثابت &a, U ثابت &b)
	{
		log_assert(a.سائيز() == b.سائيز());
		لكل (صحيح i = 0; i != Get_سائيز(a); ++i)
			جوڙيو(Drive_ٻٽو_انگ(a[i]), Drive_ٻٽو_انگ(b[i]));
	}


	// Specialized version that avoids unpacking
	فراغ جوڙيو(SigSpec ثابت &a, SigSpec ثابت &b);

خاص:
	فراغ جوڙيو_پورٽ(Cell *cell, IdString پورٽ, SigSpec ثابت &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<Drive_ٻٽو_انگ_Id> orient_undirected_ڏٺل;
	pool<Drive_ٻٽو_انگ_Id> orient_undirected_drivers;
	dict<Drive_ٻٽو_انگ_Id, صحيح> orient_undirected_مفاصلو;

	فراغ orient_undirected(Drive_ٻٽو_انگ_Id id);

عام:
	Drive_ٻٽو_انگ عامل()(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ);

	DriveSpec عامل()(DriveSpec spec);

خاص:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		أرجع wire->has_attribute(ID(keep));
	}
};

مضمن Hasher Drive_ٻٽو_انگ_Wire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(وٿي);
	أرجع h;
}

مضمن Hasher Drive_ٻٽو_انگ_پورٽ::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورٽ);
	h.eat(وٿي);
	أرجع h;
}

مضمن Hasher Drive_ٻٽو_انگ_Marker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(وٿي);
	أرجع h;
}

مضمن Hasher Drive_ٻٽو_انگ_گڻو::hash_into(Hasher h) ثابت
{
	h.eat(گڻو_3);
	أرجع h;
}

مضمن Hasher Drive_ٻٽو_انگ::hash_into(Hasher h) ثابت
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
	حالة DriveType::پورٽ_2:
		h.eat(پورٽ_3);
		اكسر;
	حالة DriveType::MARKER:
		h.eat(marker_);
		اكسر;
	حالة DriveType::گڻو:
		h.eat(گڻو_3);
		اكسر;
	}
	h.eat(type_);
	أرجع h;
}

مضمن Hasher DriveChunkWire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(ويڪر);
	h.eat(وٿي);
	أرجع h;
}

مضمن Hasher Drive_Chunk_پورٽ::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورٽ);
	h.eat(ويڪر);
	h.eat(وٿي);
	أرجع h;
}

مضمن Hasher DriveChunkMarker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(ويڪر);
	h.eat(وٿي);
	أرجع h;
}

مضمن Hasher Drive_Chunk_گڻو::hash_into(Hasher h) ثابت
{
	h.eat(ويڪر_3);
	h.eat(گڻو_3);
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
	حالة DriveType::پورٽ_2:
		h.eat(پورٽ_3);
		اكسر;
	حالة DriveType::MARKER:
		h.eat(marker_);
		اكسر;
	حالة DriveType::گڻو:
		h.eat(گڻو_3);
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

مضمن Hasher DriverMap::Drive_ٻٽو_انگ_Id::hash_into(Hasher h) ثابت
{
	h.eat(id);
	أرجع h;
}

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
