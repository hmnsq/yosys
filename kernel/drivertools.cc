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

#ضمن "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

Drive_ٻٽو_انگ::Drive_ٻٽو_انگ(Sig_ٻٽو_انگ ثابت &ٻٽو_انگ)
{
	إذا (ٻٽو_انگ.is_wire())
		*هذا = Drive_ٻٽو_انگ_Wire(ٻٽو_انگ.wire, ٻٽو_انگ.وٿي);
	وإلا
		*هذا = ٻٽو_انگ.معلومات;
}

فراغ Drive_ٻٽو_انگ::ملايو(Drive_ٻٽو_انگ ثابت &other)
{
	إذا (other.type_ == DriveType::NONE)
		أرجع;
	إذا (type_ == DriveType::NONE) {
		*هذا = other;
		أرجع;
	}
	إذا (type_ != DriveType::گڻو) {
		Drive_ٻٽو_انگ_گڻو multi(std::move(*هذا));
		*هذا = std::move(multi);
	}
	گڻو_2().ملايو(other);
}


فراغ Drive_ٻٽو_انگ_گڻو::ملايو(Drive_ٻٽو_انگ ثابت &single)
{
	إذا (single.type() == DriveType::NONE)
		أرجع;
	إذا (single.type() == DriveType::گڻو) {
		ملايو(single.گڻو_2());
		أرجع;
	}
	گڻو_3.emplace(single);
}

فراغ Drive_ٻٽو_انگ_گڻو::ملايو(Drive_ٻٽو_انگ &&single)
{
	إذا (single.type() == DriveType::NONE)
		أرجع;
	إذا (single.type() == DriveType::گڻو) {
		ملايو(std::move(single.گڻو_2()));
		أرجع;
	}
	گڻو_3.emplace(std::move(single));
}

Drive_ٻٽو_انگ_گڻو Drive_Chunk_گڻو::عامل[](صحيح i) ثابت
{
	Drive_ٻٽو_انگ_گڻو نتيجو;
	لكل (خودڪار ثابت &single : گڻو_3)
		نتيجو.ملايو(single[i]);
	أرجع نتيجو;
}


منطقي DriveChunkWire::can_append(Drive_ٻٽو_انگ_Wire ثابت &ٻٽو_انگ) ثابت
{
	أرجع ٻٽو_انگ.wire == wire && ٻٽو_انگ.وٿي == وٿي + ويڪر;
}

منطقي DriveChunkWire::try_append(Drive_ٻٽو_انگ_Wire ثابت &ٻٽو_انگ)
{
	إذا (!can_append(ٻٽو_انگ))
		أرجع خطأ;
	ويڪر += 1;
	أرجع سچ;
}

منطقي DriveChunkWire::try_append(DriveChunkWire ثابت &chunk)
{
	إذا (chunk.wire != wire || chunk.وٿي != وٿي + ويڪر)
		أرجع خطأ;
	ويڪر += chunk.ويڪر;
	أرجع سچ;
}

منطقي Drive_Chunk_پورٽ::can_append(Drive_ٻٽو_انگ_پورٽ ثابت &ٻٽو_انگ) ثابت
{
	أرجع ٻٽو_انگ.cell == cell && ٻٽو_انگ.پورٽ == پورٽ && ٻٽو_انگ.وٿي == وٿي + ويڪر;
}

منطقي Drive_Chunk_پورٽ::try_append(Drive_ٻٽو_انگ_پورٽ ثابت &ٻٽو_انگ)
{
	إذا (!can_append(ٻٽو_انگ))
		أرجع خطأ;
	ويڪر += 1;
	أرجع سچ;
}

منطقي Drive_Chunk_پورٽ::try_append(Drive_Chunk_پورٽ ثابت &chunk)
{
	إذا (chunk.cell != cell || chunk.پورٽ != پورٽ || chunk.وٿي != وٿي + ويڪر)
		أرجع خطأ;
	ويڪر += chunk.ويڪر;
	أرجع سچ;
}

منطقي DriveChunkMarker::can_append(Drive_ٻٽو_انگ_Marker ثابت &ٻٽو_انگ) ثابت
{
	أرجع ٻٽو_انگ.marker == marker && ٻٽو_انگ.وٿي == وٿي + ويڪر;
}

منطقي DriveChunkMarker::try_append(Drive_ٻٽو_انگ_Marker ثابت &ٻٽو_انگ)
{
	إذا (!can_append(ٻٽو_انگ))
		أرجع خطأ;
	ويڪر += 1;
	أرجع سچ;
}

منطقي DriveChunkMarker::try_append(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.marker != marker || chunk.وٿي != وٿي + ويڪر)
		أرجع خطأ;
	ويڪر += chunk.ويڪر;
	أرجع سچ;
}


منطقي Drive_Chunk_گڻو::can_append(Drive_ٻٽو_انگ_گڻو ثابت &ٻٽو_انگ) ثابت
{
	إذا (ٻٽو_انگ.گڻو_2().سائيز() != گڻو_3.سائيز())
		أرجع خطأ;

	صحيح const_drivers = 0;
	لكل (DriveChunk ثابت &single : گڻو_3)
		إذا (single.is_constant())
			const_drivers += 1;

	إذا (const_drivers > 1)
		أرجع خطأ;

	لكل (Drive_ٻٽو_انگ ثابت &single : ٻٽو_انگ.گڻو_2())
		إذا (single.is_constant())
			const_drivers -= 1;

	إذا (const_drivers != 0)
		أرجع خطأ;

	لكل (DriveChunk ثابت &single : گڻو_3)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
			} اكسر;
			حالة DriveType::WIRE: {
				خودڪار ثابت &wire = single.wire();
				Drive_ٻٽو_انگ ايندڙ = Drive_ٻٽو_انگ_Wire(wire.wire, wire.وٿي + wire.ويڪر);
				إذا (!ٻٽو_انگ.گڻو_2().ڳڻپ(ايندڙ))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::پورٽ_2: {
				خودڪار ثابت &پورٽ = single.پورٽ();
				Drive_ٻٽو_انگ ايندڙ = Drive_ٻٽو_انگ_پورٽ(پورٽ.cell, پورٽ.پورٽ, پورٽ.وٿي + پورٽ.ويڪر);
				إذا (!ٻٽو_انگ.گڻو_2().ڳڻپ(ايندڙ))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				خودڪار ثابت &marker = single.marker();
				Drive_ٻٽو_انگ ايندڙ = Drive_ٻٽو_انگ_Marker(marker.marker, marker.وٿي + marker.ويڪر);
				إذا (!ٻٽو_انگ.گڻو_2().ڳڻپ(ايندڙ))
					أرجع خطأ;
			} اكسر;
			افتراضي:
				أرجع خطأ;
		}
	}

	أرجع سچ;
}

منطقي Drive_Chunk_گڻو::can_append(Drive_Chunk_گڻو ثابت &chunk) ثابت
{
	إذا (chunk.گڻو_2().سائيز() != گڻو_3.سائيز())
		أرجع خطأ;

	صحيح const_drivers = 0;
	لكل (DriveChunk ثابت &single : گڻو_3)
		إذا (single.is_constant())
			const_drivers += 1;

	إذا (const_drivers > 1)
		أرجع خطأ;

	لكل (DriveChunk ثابت &single : chunk.گڻو_2())
		إذا (single.is_constant())
			const_drivers -= 1;

	إذا (const_drivers != 0)
		أرجع خطأ;

	لكل (DriveChunk ثابت &single : گڻو_3)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
			} اكسر;
			حالة DriveType::WIRE: {
				خودڪار ثابت &wire = single.wire();
				DriveChunk ايندڙ = DriveChunkWire(wire.wire, wire.وٿي + wire.ويڪر, chunk.سائيز());
				إذا (!chunk.گڻو_2().ڳڻپ(ايندڙ))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::پورٽ_2: {
				خودڪار ثابت &پورٽ = single.پورٽ();
				DriveChunk ايندڙ = Drive_Chunk_پورٽ(پورٽ.cell, پورٽ.پورٽ, پورٽ.وٿي + پورٽ.ويڪر, chunk.سائيز());
				إذا (!chunk.گڻو_2().ڳڻپ(ايندڙ))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				خودڪار ثابت &marker = single.marker();
				DriveChunk ايندڙ = DriveChunkMarker(marker.marker, marker.وٿي + marker.ويڪر, chunk.سائيز());
				إذا (!chunk.گڻو_2().ڳڻپ(ايندڙ))
					أرجع خطأ;
			} اكسر;
			افتراضي:
				أرجع خطأ;
		}
	}

	أرجع سچ;
}

منطقي Drive_Chunk_گڻو::try_append(Drive_ٻٽو_انگ_گڻو ثابت &ٻٽو_انگ)
{
	إذا (!can_append(ٻٽو_انگ))
		أرجع خطأ;
	ويڪر_3 += 1;
	State constant;

	لكل (Drive_ٻٽو_انگ ثابت &single : ٻٽو_انگ.گڻو_2())
		إذا (single.is_constant())
			constant = single.constant();

	لكل (DriveChunk &single : گڻو_3)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} اكسر;
			حالة DriveType::WIRE: {
				single.wire().ويڪر += 1;
			} اكسر;
			حالة DriveType::پورٽ_2: {
				single.پورٽ().ويڪر += 1;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().ويڪر += 1;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	أرجع سچ;
}

منطقي Drive_Chunk_گڻو::try_append(Drive_Chunk_گڻو ثابت &chunk)
{
	إذا (!can_append(chunk))
		أرجع خطأ;
	صحيح ويڪر = chunk.سائيز();
	ويڪر_3 += ويڪر;
	Const constant;

	لكل (DriveChunk ثابت &single : chunk.گڻو_2())
		إذا (single.is_constant())
			constant = single.constant();

	لكل (DriveChunk &single : گڻو_3)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
				single.constant().append(constant);
			} اكسر;
			حالة DriveType::WIRE: {
				single.wire().ويڪر += ويڪر;
			} اكسر;
			حالة DriveType::پورٽ_2: {
				single.پورٽ().ويڪر += ويڪر;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().ويڪر += ويڪر;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	أرجع سچ;
}

منطقي DriveChunk::can_append(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ) ثابت
{
	إذا (سائيز() == 0)
		أرجع سچ;
	إذا (ٻٽو_انگ.type() != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			أرجع سچ;
		حالة DriveType::CONSTANT:
			أرجع سچ;
		حالة DriveType::WIRE:
			أرجع wire_.can_append(ٻٽو_انگ.wire());
		حالة DriveType::پورٽ_2:
			أرجع پورٽ_3.can_append(ٻٽو_انگ.پورٽ());
		حالة DriveType::گڻو:
			أرجع گڻو_3.can_append(ٻٽو_انگ.گڻو_2());
		افتراضي:
			log_abort();
	}
}

منطقي DriveChunk::try_append(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ)
{
	إذا (سائيز() == 0)
		*هذا = ٻٽو_انگ;
	إذا (ٻٽو_انگ.type() != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			none_ += 1;
			أرجع سچ;
		حالة DriveType::CONSTANT:
			constant_.append(RTLIL::Const(ٻٽو_انگ.constant()));
			أرجع سچ;
		حالة DriveType::WIRE:
			أرجع wire_.try_append(ٻٽو_انگ.wire());
		حالة DriveType::پورٽ_2:
			أرجع پورٽ_3.try_append(ٻٽو_انگ.پورٽ());
		حالة DriveType::گڻو:
			أرجع گڻو_3.try_append(ٻٽو_انگ.گڻو_2());
		افتراضي:
			log_abort();
	}
}


منطقي DriveChunk::try_append(DriveChunk ثابت &chunk)
{
	إذا (سائيز() == 0)
		*هذا = chunk;
	إذا (chunk.type_ != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			none_ += chunk.none_;
			أرجع سچ;
		حالة DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			أرجع سچ;
		حالة DriveType::WIRE:
			أرجع wire_.try_append(chunk.wire());
		حالة DriveType::پورٽ_2:
			أرجع پورٽ_3.try_append(chunk.پورٽ());
		حالة DriveType::MARKER:
			أرجع marker_.try_append(chunk.marker());
		حالة DriveType::گڻو:
			أرجع گڻو_3.try_append(chunk.گڻو_2());
	}
	log_abort();
}

فراغ DriveSpec::append(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ)
{
	hash_ = 0;
	إذا (!packed()) {
		بٽ_2.ڌڪو_پٺ(ٻٽو_انگ);
		ويڪر_3 += 1;
		أرجع;
	}

	إذا (chunks_.خالي_جايون() || !chunks_.پٺ().try_append(ٻٽو_انگ))
		chunks_.emplace_پٺ(ٻٽو_انگ);
	ويڪر_3 += 1;
}

فراغ DriveSpec::append(DriveChunk ثابت &chunk)
{
	hash_ = 0;
	pack();
	إذا (chunks_.خالي_جايون() || !chunks_.پٺ().try_append(chunk))
		chunks_.emplace_پٺ(chunk);
	ويڪر_3 += chunk.سائيز();
}

فراغ DriveSpec::pack() ثابت {
	إذا (بٽ_2.خالي_جايون())
		أرجع;
	std::ويڪٽر<Drive_ٻٽو_انگ> بٽ(std::move(بٽ_2));
	لكل (خودڪار &ٻٽو_انگ : بٽ)
		إذا (chunks_.خالي_جايون() || !chunks_.پٺ().try_append(ٻٽو_انگ))
			chunks_.emplace_پٺ(std::move(ٻٽو_انگ));
}

فراغ DriveSpec::unpack() ثابت {
	إذا (chunks_.خالي_جايون())
		أرجع;
	لكل (خودڪار &chunk : chunks_)
	{
		لكل (صحيح i = 0, ويڪر = chunk.سائيز(); i != ويڪر; ++i)
		{
			بٽ_2.emplace_پٺ(chunk[i]);
		}
	}
	chunks_.clear();
}

فراغ DriveSpec::compute_ويڪر()
{
	ويڪر_3 = 0;
	لكل (خودڪار ثابت &chunk : chunks_)
		ويڪر_3 += chunk.سائيز();
}

فراغ DriverMap::Drive_ٻٽو_انگ_گراف::جوڙيو_ڪنارو(Drive_ٻٽو_انگ_Id src, Drive_ٻٽو_انگ_Id dst)
{
	إذا (پهريون_ڪنارا.emplace(src, dst).پهريون->ٻيو == dst)
		أرجع;
	إذا (ٻيو_ڪنارا.emplace(src, dst).پهريون->ٻيو == dst)
		أرجع;
	وڌيڪ_ڪنارا[src].emplace(dst);
}

DriverMap::Drive_ٻٽو_انگ_Id DriverMap::Drive_ٻٽو_انگ_گراف::ڪڍو_ڪنارو(Drive_ٻٽو_انگ_Id src)
{
	// TODO unused I think?
	خودڪار مليو_وڌيڪ = وڌيڪ_ڪنارا.ڳوليو(src);
	إذا (مليو_وڌيڪ != وڌيڪ_ڪنارا.پڇاڙي()) {
		خودڪار نتيجو = مليو_وڌيڪ->ٻيو.ڪڍو();
		إذا (مليو_وڌيڪ->ٻيو.خالي_جايون())
			وڌيڪ_ڪنارا.erase(مليو_وڌيڪ);
		أرجع نتيجو;
	}

	خودڪار مليو_ٻيو = ٻيو_ڪنارا.ڳوليو(src);
	إذا (مليو_ٻيو != ٻيو_ڪنارا.پڇاڙي()) {
		خودڪار نتيجو = مليو_ٻيو->ٻيو;
		ٻيو_ڪنارا.erase(مليو_ٻيو);
		أرجع نتيجو;
	}

	خودڪار مليو_پهريون = پهريون_ڪنارا.ڳوليو(src);
	إذا (مليو_پهريون != پهريون_ڪنارا.پڇاڙي()) {
		خودڪار نتيجو = مليو_پهريون->ٻيو;
		پهريون_ڪنارا.erase(مليو_پهريون);
		أرجع نتيجو;
	}

	أرجع Drive_ٻٽو_انگ_Id();
}

فراغ DriverMap::Drive_ٻٽو_انگ_گراف::clear(Drive_ٻٽو_انگ_Id src)
{
	پهريون_ڪنارا.erase(src);
	ٻيو_ڪنارا.erase(src);
	وڌيڪ_ڪنارا.erase(src);
}

منطقي DriverMap::Drive_ٻٽو_انگ_گراف::شامل_آهي(Drive_ٻٽو_انگ_Id src)
{
	أرجع پهريون_ڪنارا.ڳڻپ(src);
}

صحيح DriverMap::Drive_ٻٽو_انگ_گراف::ڳڻپ(Drive_ٻٽو_انگ_Id src)
{
	إذا (!پهريون_ڪنارا.ڳڻپ(src))
		أرجع 0;
	إذا (!ٻيو_ڪنارا.ڳڻپ(src))
		أرجع 1;
	خودڪار مليو = وڌيڪ_ڪنارا.ڳوليو(src);
	إذا (مليو == وڌيڪ_ڪنارا.پڇاڙي())
		أرجع 2;
	أرجع Get_سائيز(مليو->ٻيو) + 2;
}

DriverMap::Drive_ٻٽو_انگ_Id DriverMap::Drive_ٻٽو_انگ_گراف::at(Drive_ٻٽو_انگ_Id src, صحيح اشاريو)
{
	إذا (اشاريو == 0)
		أرجع پهريون_ڪنارا.at(src);
	وإلا إذا (اشاريو == 1)
		أرجع ٻيو_ڪنارا.at(src);
	وإلا
		أرجع *وڌيڪ_ڪنارا.at(src).element(اشاريو - 2);
}


DriverMap::ٻٽو_انگ_گهڻائي DriverMap::ٻٽو_انگ_گهڻائي_2(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ)
{
	بدل (ٻٽو_انگ.type())
	{
		حالة DriveType::NONE:
			أرجع ٻٽو_انگ_گهڻائي::NONE;
		حالة DriveType::CONSTANT:
			// TODO how to handle Sx here?
			أرجع ٻٽو_انگ.constant() == State::Sz ? ٻٽو_انگ_گهڻائي::NONE : ٻٽو_انگ_گهڻائي::DRIVER;
		حالة DriveType::WIRE: {
			خودڪار ثابت &wire = ٻٽو_انگ.wire();
			منطقي driver = wire.wire->پورٽ_داخلا;
			منطقي driven = wire.wire->پورٽ_output;

			إذا (driver && !driven)
				أرجع ٻٽو_انگ_گهڻائي::DRIVER;
			وإلا إذا (driven && !driver)
				أرجع ٻٽو_انگ_گهڻائي::DRIVEN;
			وإلا إذا (driver && driven)
				أرجع ٻٽو_انگ_گهڻائي::TRISTATE;
			وإلا
				أرجع keep_wire(ٻٽو_انگ.wire().wire) ? ٻٽو_انگ_گهڻائي::KEEP : ٻٽو_انگ_گهڻائي::NONE;
		}
		حالة DriveType::پورٽ_2: {
			خودڪار ثابت &پورٽ = ٻٽو_انگ.پورٽ();
			منطقي driver = celltypes.cell_output(پورٽ.cell->type, پورٽ.پورٽ);
			منطقي driven = celltypes.cell_داخلا(پورٽ.cell->type, پورٽ.پورٽ);
			إذا (driver && !driven)
				أرجع ٻٽو_انگ_گهڻائي::DRIVER;
			وإلا إذا (driven && !driver)
				أرجع ٻٽو_انگ_گهڻائي::DRIVEN_UNIQUE;
			وإلا
				أرجع ٻٽو_انگ_گهڻائي::TRISTATE;
		}
		حالة DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		افتراضي:
			log_abort();
	}
}

DriverMap::Drive_ٻٽو_انگ_Id DriverMap::id_from_drive_ٻٽو_انگ(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ)
{
	بدل (ٻٽو_انگ.type())
	{
		حالة DriveType::NONE:
			أرجع -1;
		حالة DriveType::CONSTANT:
			أرجع (صحيح)ٻٽو_انگ.constant();
		حالة DriveType::WIRE: {
			خودڪار ثابت &wire_ٻٽو_انگ = ٻٽو_انگ.wire();
			صحيح وٿي = ايندڙ_وٿي;
			خودڪار insertion = wire_offsets.emplace(wire_ٻٽو_انگ.wire, وٿي);
			إذا (insertion.ٻيو) {
				إذا (wire_ٻٽو_انگ.wire->ويڪر == 1) {
					log_assert(wire_ٻٽو_انگ.وٿي == 0);
					isolated_drive_بٽ.emplace(وٿي, ٻٽو_انگ);
				} وإلا
					drive_بٽ.emplace(وٿي, Drive_ٻٽو_انگ_Wire(wire_ٻٽو_انگ.wire, 0));
				ايندڙ_وٿي += wire_ٻٽو_انگ.wire->ويڪر;
			}
			أرجع insertion.پهريون->ٻيو.id + wire_ٻٽو_انگ.وٿي;
		}
		حالة DriveType::پورٽ_2: {
			خودڪار ثابت &پورٽ_ٻٽو_انگ = ٻٽو_انگ.پورٽ();
			خودڪار ڪنجي = std::make_pair(پورٽ_ٻٽو_انگ.cell, پورٽ_ٻٽو_انگ.پورٽ);
			صحيح وٿي = ايندڙ_وٿي;
			خودڪار insertion = پورٽ_offsets.emplace(ڪنجي, وٿي);
			إذا (insertion.ٻيو) {
				صحيح ويڪر = پورٽ_ٻٽو_انگ.cell->connections().at(پورٽ_ٻٽو_انگ.پورٽ).سائيز();
				إذا (ويڪر == 1 && وٿي == 0) {
					log_assert(پورٽ_ٻٽو_انگ.وٿي == 0);
					isolated_drive_بٽ.emplace(وٿي, ٻٽو_انگ);
				} وإلا
					drive_بٽ.emplace(وٿي, Drive_ٻٽو_انگ_پورٽ(پورٽ_ٻٽو_انگ.cell, پورٽ_ٻٽو_انگ.پورٽ, 0));
				ايندڙ_وٿي += ويڪر;
			}
			أرجع insertion.پهريون->ٻيو.id + پورٽ_ٻٽو_انگ.وٿي;
		}
		افتراضي:
			log_assert(خطأ && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

Drive_ٻٽو_انگ DriverMap::drive_ٻٽو_انگ_from_id(Drive_ٻٽو_انگ_Id id)
{
	خودڪار مليو_isolated = isolated_drive_بٽ.ڳوليو(id);
	إذا (مليو_isolated != isolated_drive_بٽ.پڇاڙي())
		أرجع مليو_isolated->ٻيو;

	خودڪار مليو = drive_بٽ.مٿانهون_bound(id);
	إذا (مليو == drive_بٽ.begin()) {
		أرجع id < 0 ? Drive_ٻٽو_انگ() : Drive_ٻٽو_انگ((State) id.id);
	}
	--مليو;
	Drive_ٻٽو_انگ نتيجو = مليو->ٻيو;
	إذا (نتيجو.is_wire()) {
		نتيجو.wire().وٿي += id.id - مليو->پهريون.id;
	} وإلا {
		log_assert(نتيجو.is_پورٽ());
		نتيجو.پورٽ().وٿي += id.id - مليو->پهريون.id;
	}
	أرجع نتيجو;
}

فراغ DriverMap::connect_directed_ملايو(Drive_ٻٽو_انگ_Id driven_id, Drive_ٻٽو_انگ_Id driver_id)
{
	إذا (driven_id == driver_id)
		أرجع;

	same_driver.ملايو(driven_id, driver_id);

	لكل (صحيح i = 0, پڇاڙي = connected_drivers.ڳڻپ(driven_id); i != پڇاڙي; ++i)
		connected_drivers.جوڙيو_ڪنارو(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	لكل (صحيح i = 0, پڇاڙي = connected_undirected.ڳڻپ(driven_id); i != پڇاڙي; ++i)
		connected_undirected.جوڙيو_ڪنارو(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

فراغ DriverMap::connect_directed_بفر(Drive_ٻٽو_انگ_Id driven_id, Drive_ٻٽو_انگ_Id driver_id)
{
	connected_drivers.جوڙيو_ڪنارو(driven_id, driver_id);
}

فراغ DriverMap::connect_undirected(Drive_ٻٽو_انگ_Id a_id, Drive_ٻٽو_انگ_Id b_id)
{
	connected_undirected.جوڙيو_ڪنارو(a_id, b_id);
	connected_undirected.جوڙيو_ڪنارو(b_id, a_id);
}

فراغ DriverMap::جوڙيو(Module *module)
{
	لكل (خودڪار ثابت &conn : module->connections())
		جوڙيو(conn.پهريون, conn.ٻيو);

	لكل (خودڪار cell : module->cells())
		لكل (خودڪار ثابت &conn : cell->connections())
			جوڙيو_پورٽ(cell, conn.پهريون, conn.ٻيو);
}

// Add a single bit connection to the driver map.
فراغ DriverMap::جوڙيو(Drive_ٻٽو_انگ ثابت &a, Drive_ٻٽو_انگ ثابت &b)
{
	Drive_ٻٽو_انگ_Id a_id = id_from_drive_ٻٽو_انگ(a);
	Drive_ٻٽو_انگ_Id b_id = id_from_drive_ٻٽو_انگ(b);

	Drive_ٻٽو_انگ_Id orig_a_id = a_id;
	Drive_ٻٽو_انگ_Id orig_b_id = b_id;

	a_id = same_driver.ڳوليو(a_id);
	b_id = same_driver.ڳوليو(b_id);

	إذا (a_id == b_id)
		أرجع;

	ٻٽو_انگ_گهڻائي a_گهڻائي = ٻٽو_انگ_گهڻائي_2(orig_a_id == a_id ? a : drive_ٻٽو_انگ_from_id(a_id));
	ٻٽو_انگ_گهڻائي b_گهڻائي = ٻٽو_انگ_گهڻائي_2(orig_b_id == b_id ? b : drive_ٻٽو_انگ_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	إذا (a_گهڻائي == ٻٽو_انگ_گهڻائي::NONE && !(b_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN_UNIQUE || b_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN))
		connect_directed_ملايو(a_id, b_id);
	وإلا إذا (b_گهڻائي == ٻٽو_انگ_گهڻائي::NONE && !(a_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN_UNIQUE || a_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN))
		connect_directed_ملايو(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	وإلا إذا (a_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN_UNIQUE && !(b_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN_UNIQUE || b_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN))
		connect_directed_بفر(a_id, b_id);
	وإلا إذا (b_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN_UNIQUE && !(a_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN_UNIQUE || a_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVEN))
		connect_directed_بفر(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	وإلا إذا (a_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVER)
		connect_directed_بفر(b_id, a_id);
	وإلا إذا (b_گهڻائي == ٻٽو_انگ_گهڻائي::DRIVER)
		connect_directed_بفر(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	وإلا
		connect_undirected(a_id, b_id);

	أرجع;
}

// Specialized version that avoids unpacking
فراغ DriverMap::جوڙيو(SigSpec ثابت &a, SigSpec ثابت &b)
{
	log_assert(a.سائيز() == b.سائيز());
	خودڪار ثابت &a_chunks = a.chunks();
	خودڪار ثابت &b_chunks = b.chunks();

	خودڪار a_chunk = a_chunks.begin();
	خودڪار a_پڇاڙي = a_chunks.پڇاڙي();
	صحيح a_وٿي = 0;

	خودڪار b_chunk = b_chunks.begin();
	صحيح b_وٿي = 0;

	SigChunk tmp_a, tmp_b;
	طالما (a_chunk != a_پڇاڙي) {
		صحيح a_ويڪر = a_chunk->ويڪر - a_وٿي;
		إذا (a_ويڪر == 0) {
			a_وٿي = 0;
			++a_chunk;
			استمر;
		}
		صحيح b_ويڪر = b_chunk->ويڪر - b_وٿي;
		إذا (b_ويڪر == 0) {
			b_وٿي = 0;
			++b_chunk;
			استمر;
		}
		صحيح ويڪر = std::min(a_ويڪر, b_ويڪر);
		log_assert(ويڪر > 0);

		SigChunk ثابت &a_subchunk =
			a_وٿي == 0 && a_ويڪر == ويڪر ? *a_chunk : a_chunk->ڪڍو_2(a_وٿي, ويڪر);
		SigChunk ثابت &b_subchunk =
			b_وٿي == 0 && b_ويڪر == ويڪر ? *b_chunk : b_chunk->ڪڍو_2(b_وٿي, ويڪر);

		جوڙيو(a_subchunk, b_subchunk);

		a_وٿي += ويڪر;
		b_وٿي += ويڪر;
	}
}

فراغ DriverMap::جوڙيو_پورٽ(Cell *cell, IdString پورٽ, SigSpec ثابت &b)
{
	صحيح وٿي = 0;
	لكل (خودڪار ثابت &chunk : b.chunks()) {
		جوڙيو(chunk, Drive_Chunk_پورٽ(cell, پورٽ, وٿي, chunk.ويڪر));
		وٿي += chunk.سائيز();
	}
}

فراغ DriverMap::orient_undirected(Drive_ٻٽو_انگ_Id id)
{
	pool<Drive_ٻٽو_انگ_Id> &ڏٺل = orient_undirected_ڏٺل;
	pool<Drive_ٻٽو_انگ_Id> &drivers = orient_undirected_drivers;
	dict<Drive_ٻٽو_انگ_Id, صحيح> &مفاصلو = orient_undirected_مفاصلو;
	ڏٺل.clear();
	drivers.clear();

	ڏٺل.emplace(id);

	لكل (صحيح pos = 0; pos < Get_سائيز(ڏٺل); ++pos) {
		Drive_ٻٽو_انگ_Id current = *ڏٺل.element(ڏٺل.سائيز() - 1 - pos);
		Drive_ٻٽو_انگ ٻٽو_انگ = drive_ٻٽو_انگ_from_id(current);

		ٻٽو_انگ_گهڻائي گهڻائي = ٻٽو_انگ_گهڻائي_2(ٻٽو_انگ);

		إذا (گهڻائي == ٻٽو_انگ_گهڻائي::DRIVER || گهڻائي == ٻٽو_انگ_گهڻائي::TRISTATE)
			drivers.emplace(current);

		إذا (connected_drivers.شامل_آهي(current))
			drivers.emplace(current);

		صحيح undirected_driver_ڳڻپ = connected_undirected.ڳڻپ(current);

		لكل (صحيح i = 0; i != undirected_driver_ڳڻپ; ++i)
			ڏٺل.emplace(same_driver.ڳوليو(connected_undirected.at(current, i)));
	}

	إذا (drivers.خالي_جايون())
		لكل (خودڪار ڏٺل_id : ڏٺل)
			drivers.emplace(ڏٺل_id);

	لكل (خودڪار driver : drivers)
	{
		مفاصلو.clear();
		مفاصلو.emplace(driver, 0);

		لكل (صحيح pos = 0; pos < Get_سائيز(مفاصلو); ++pos) {
			خودڪار current_it = مفاصلو.element(مفاصلو.سائيز() - 1 - pos);

			Drive_ٻٽو_انگ_Id current = current_it->پهريون;
			صحيح undirected_driver_ڳڻپ = connected_undirected.ڳڻپ(current);

			لكل (صحيح i = 0; i != undirected_driver_ڳڻپ; ++i)
			{
				Drive_ٻٽو_انگ_Id ايندڙ = same_driver.ڳوليو(connected_undirected.at(current, i));
				خودڪار emplaced = مفاصلو.emplace(ايندڙ, current_it->ٻيو + 1);
				إذا (emplaced.پهريون->ٻيو == current_it->ٻيو + 1)
					connected_oriented.جوڙيو_ڪنارو(ايندڙ, current);
			}
		}
	}

	لكل (خودڪار ڏٺل_id : ڏٺل)
		oriented_present.emplace(ڏٺل_id);
}

Drive_ٻٽو_انگ DriverMap::عامل()(Drive_ٻٽو_انگ ثابت &ٻٽو_انگ)
{
	إذا (ٻٽو_انگ.type() == DriveType::MARKER || ٻٽو_انگ.type() == DriveType::NONE)
		أرجع ٻٽو_انگ;
	إذا (ٻٽو_انگ.type() == DriveType::گڻو)
	{
		Drive_ٻٽو_انگ نتيجو;
		لكل (خودڪار ثابت &inner : ٻٽو_انگ.گڻو_2().گڻو_2())
			نتيجو.ملايو((*هذا)(inner));
		أرجع نتيجو;
	}

	Drive_ٻٽو_انگ_Id ٻٽو_انگ_id = id_from_drive_ٻٽو_انگ(ٻٽو_انگ);

	Drive_ٻٽو_انگ_Id ٻٽو_انگ_repr_id = same_driver.ڳوليو(ٻٽو_انگ_id);

	Drive_ٻٽو_انگ ٻٽو_انگ_repr = drive_ٻٽو_انگ_from_id(ٻٽو_انگ_repr_id);

	ٻٽو_انگ_گهڻائي گهڻائي = ٻٽو_انگ_گهڻائي_2(ٻٽو_انگ_repr);

	إذا (گهڻائي == ٻٽو_انگ_گهڻائي::KEEP && ٻٽو_انگ_repr_id != ٻٽو_انگ_id)
		أرجع ٻٽو_انگ_repr;

	صحيح implicit_driver_ڳڻپ = connected_drivers.ڳڻپ(ٻٽو_انگ_repr_id);
	إذا (connected_undirected.شامل_آهي(ٻٽو_انگ_repr_id) && !oriented_present.ڳڻپ(ٻٽو_انگ_repr_id))
		orient_undirected(ٻٽو_انگ_repr_id);

	Drive_ٻٽو_انگ driver;

	إذا (گهڻائي == ٻٽو_انگ_گهڻائي::DRIVER || گهڻائي == ٻٽو_انگ_گهڻائي::TRISTATE)
		driver = ٻٽو_انگ_repr;

	لكل (صحيح i = 0; i != implicit_driver_ڳڻپ; ++i)
		driver.ملايو(drive_ٻٽو_انگ_from_id(connected_drivers.at(ٻٽو_انگ_repr_id, i)));

	صحيح oriented_driver_ڳڻپ = connected_oriented.ڳڻپ(ٻٽو_انگ_repr_id);
	لكل (صحيح i = 0; i != oriented_driver_ڳڻپ; ++i)
		driver.ملايو(drive_ٻٽو_انگ_from_id(connected_oriented.at(ٻٽو_انگ_repr_id, i)));

	أرجع driver;
}

DriveSpec DriverMap::عامل()(DriveSpec spec)
{
	DriveSpec نتيجو;

	لكل (صحيح i = 0, ويڪر = spec.سائيز(); i != ويڪر; ++i)
		نتيجو.append((*هذا)(spec[i]));

	أرجع نتيجو;
}

std::string log_signal(DriveChunkWire ثابت &chunk)
{
	std::string id = chunk.wire->name.unescape();
	إذا (chunk.is_سڄو())
		أرجع id;
	إذا (chunk.ويڪر == 1)
		أرجع stringf("%s [%d]", id, chunk.وٿي);
	أرجع stringf("%s [%d:%d]", id, chunk.وٿي + chunk.ويڪر - 1, chunk.وٿي);
}


std::string log_signal(Drive_Chunk_پورٽ ثابت &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string پورٽ_id = chunk.cell->twines().unescaped_str(chunk.پورٽ);
	إذا (chunk.is_سڄو())
		أرجع stringf("%s <%s>", cell_id, پورٽ_id);
	إذا (chunk.ويڪر == 1)
		أرجع stringf("%s <%s> [%d]", cell_id, پورٽ_id, chunk.وٿي);
	أرجع stringf("%s <%s> [%d:%d]", cell_id, پورٽ_id, chunk.وٿي + chunk.ويڪر - 1, chunk.وٿي);
}

std::string log_signal(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.ويڪر == 1)
		أرجع stringf("<marker %d> [%d]", chunk.marker, chunk.وٿي);
	أرجع stringf("<marker %d> [%d:%d]", chunk.marker, chunk.وٿي + chunk.ويڪر - 1, chunk.وٿي);
}

std::string log_signal(DriveChunk ثابت &chunk)
{
	بدل (chunk.type())
	{
		حالة DriveType::NONE:
			أرجع stringf("<none x%d>", chunk.سائيز());
		حالة DriveType::CONSTANT:
			أرجع log_const(chunk.constant());
		حالة DriveType::WIRE:
			أرجع log_signal(chunk.wire());
		حالة DriveType::پورٽ_2:
			أرجع log_signal(chunk.پورٽ());
		حالة DriveType::MARKER:
			أرجع log_signal(chunk.marker());
		حالة DriveType::گڻو: {
			std::string str = "<multiple";
			ثابت حرف *sep = " ";
			لكل (خودڪار ثابت &single : chunk.گڻو_2().گڻو_2()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			أرجع str;
		}
		افتراضي:
			log_abort();
	}
}

std::string log_signal(DriveSpec ثابت &spec)
{
	خودڪار &chunks = spec.chunks();
	إذا (chunks.خالي_جايون())
		أرجع "{}";
	إذا (chunks.سائيز() == 1)
		أرجع log_signal(chunks[0]);

	std::string str;
	ثابت حرف *sep = "{ ";

	لكل (خودڪار i = chunks.rbegin(), پڇاڙي = chunks.rend(); i != پڇاڙي; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	أرجع str;
}

YOSYS_NAMESPACE_پڇاڙي
