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

Drive_ثنائی_ہندسہ::Drive_ثنائی_ہندسہ(Sig_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	إذا (ثنائی_ہندسہ.is_wire())
		*هذا = Drive_ثنائی_ہندسہ_Wire(ثنائی_ہندسہ.wire, ثنائی_ہندسہ.وتھ);
	وإلا
		*هذا = ثنائی_ہندسہ.انکڑے;
}

فراغ Drive_ثنائی_ہندسہ::ملاؤ(Drive_ثنائی_ہندسہ ثابت &other)
{
	إذا (other.type_ == DriveType::NONE)
		أرجع;
	إذا (type_ == DriveType::NONE) {
		*هذا = other;
		أرجع;
	}
	إذا (type_ != DriveType::مضرب) {
		Drive_ثنائی_ہندسہ_مضرب multi(std::move(*هذا));
		*هذا = std::move(multi);
	}
	مضرب_2().ملاؤ(other);
}


فراغ Drive_ثنائی_ہندسہ_مضرب::ملاؤ(Drive_ثنائی_ہندسہ ثابت &single)
{
	إذا (single.type() == DriveType::NONE)
		أرجع;
	إذا (single.type() == DriveType::مضرب) {
		ملاؤ(single.مضرب_2());
		أرجع;
	}
	مضرب_3.emplace(single);
}

فراغ Drive_ثنائی_ہندسہ_مضرب::ملاؤ(Drive_ثنائی_ہندسہ &&single)
{
	إذا (single.type() == DriveType::NONE)
		أرجع;
	إذا (single.type() == DriveType::مضرب) {
		ملاؤ(std::move(single.مضرب_2()));
		أرجع;
	}
	مضرب_3.emplace(std::move(single));
}

Drive_ثنائی_ہندسہ_مضرب Drive_Chunk_مضرب::عامل[](صحيح i) ثابت
{
	Drive_ثنائی_ہندسہ_مضرب نتیجہ;
	لكل (خودکار ثابت &single : مضرب_3)
		نتیجہ.ملاؤ(single[i]);
	أرجع نتیجہ;
}


منطقي DriveChunkWire::can_append(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ) ثابت
{
	أرجع ثنائی_ہندسہ.wire == wire && ثنائی_ہندسہ.وتھ == وتھ + چوڑائی;
}

منطقي DriveChunkWire::try_append(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ)
{
	إذا (!can_append(ثنائی_ہندسہ))
		أرجع خطأ;
	چوڑائی += 1;
	أرجع سچ;
}

منطقي DriveChunkWire::try_append(DriveChunkWire ثابت &chunk)
{
	إذا (chunk.wire != wire || chunk.وتھ != وتھ + چوڑائی)
		أرجع خطأ;
	چوڑائی += chunk.چوڑائی;
	أرجع سچ;
}

منطقي Drive_Chunk_پورٹ::can_append(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ) ثابت
{
	أرجع ثنائی_ہندسہ.cell == cell && ثنائی_ہندسہ.پورٹ == پورٹ && ثنائی_ہندسہ.وتھ == وتھ + چوڑائی;
}

منطقي Drive_Chunk_پورٹ::try_append(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ)
{
	إذا (!can_append(ثنائی_ہندسہ))
		أرجع خطأ;
	چوڑائی += 1;
	أرجع سچ;
}

منطقي Drive_Chunk_پورٹ::try_append(Drive_Chunk_پورٹ ثابت &chunk)
{
	إذا (chunk.cell != cell || chunk.پورٹ != پورٹ || chunk.وتھ != وتھ + چوڑائی)
		أرجع خطأ;
	چوڑائی += chunk.چوڑائی;
	أرجع سچ;
}

منطقي DriveChunkMarker::can_append(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ) ثابت
{
	أرجع ثنائی_ہندسہ.marker == marker && ثنائی_ہندسہ.وتھ == وتھ + چوڑائی;
}

منطقي DriveChunkMarker::try_append(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ)
{
	إذا (!can_append(ثنائی_ہندسہ))
		أرجع خطأ;
	چوڑائی += 1;
	أرجع سچ;
}

منطقي DriveChunkMarker::try_append(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.marker != marker || chunk.وتھ != وتھ + چوڑائی)
		أرجع خطأ;
	چوڑائی += chunk.چوڑائی;
	أرجع سچ;
}


منطقي Drive_Chunk_مضرب::can_append(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ) ثابت
{
	إذا (ثنائی_ہندسہ.مضرب_2().ناپ() != مضرب_3.ناپ())
		أرجع خطأ;

	صحيح const_drivers = 0;
	لكل (DriveChunk ثابت &single : مضرب_3)
		إذا (single.is_constant())
			const_drivers += 1;

	إذا (const_drivers > 1)
		أرجع خطأ;

	لكل (Drive_ثنائی_ہندسہ ثابت &single : ثنائی_ہندسہ.مضرب_2())
		إذا (single.is_constant())
			const_drivers -= 1;

	إذا (const_drivers != 0)
		أرجع خطأ;

	لكل (DriveChunk ثابت &single : مضرب_3)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
			} اكسر;
			حالة DriveType::WIRE: {
				خودکار ثابت &wire = single.wire();
				Drive_ثنائی_ہندسہ اگلا = Drive_ثنائی_ہندسہ_Wire(wire.wire, wire.وتھ + wire.چوڑائی);
				إذا (!ثنائی_ہندسہ.مضرب_2().گنتی(اگلا))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::پورٹ_2: {
				خودکار ثابت &پورٹ = single.پورٹ();
				Drive_ثنائی_ہندسہ اگلا = Drive_ثنائی_ہندسہ_پورٹ(پورٹ.cell, پورٹ.پورٹ, پورٹ.وتھ + پورٹ.چوڑائی);
				إذا (!ثنائی_ہندسہ.مضرب_2().گنتی(اگلا))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				خودکار ثابت &marker = single.marker();
				Drive_ثنائی_ہندسہ اگلا = Drive_ثنائی_ہندسہ_Marker(marker.marker, marker.وتھ + marker.چوڑائی);
				إذا (!ثنائی_ہندسہ.مضرب_2().گنتی(اگلا))
					أرجع خطأ;
			} اكسر;
			افتراضي:
				أرجع خطأ;
		}
	}

	أرجع سچ;
}

منطقي Drive_Chunk_مضرب::can_append(Drive_Chunk_مضرب ثابت &chunk) ثابت
{
	إذا (chunk.مضرب_2().ناپ() != مضرب_3.ناپ())
		أرجع خطأ;

	صحيح const_drivers = 0;
	لكل (DriveChunk ثابت &single : مضرب_3)
		إذا (single.is_constant())
			const_drivers += 1;

	إذا (const_drivers > 1)
		أرجع خطأ;

	لكل (DriveChunk ثابت &single : chunk.مضرب_2())
		إذا (single.is_constant())
			const_drivers -= 1;

	إذا (const_drivers != 0)
		أرجع خطأ;

	لكل (DriveChunk ثابت &single : مضرب_3)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
			} اكسر;
			حالة DriveType::WIRE: {
				خودکار ثابت &wire = single.wire();
				DriveChunk اگلا = DriveChunkWire(wire.wire, wire.وتھ + wire.چوڑائی, chunk.ناپ());
				إذا (!chunk.مضرب_2().گنتی(اگلا))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::پورٹ_2: {
				خودکار ثابت &پورٹ = single.پورٹ();
				DriveChunk اگلا = Drive_Chunk_پورٹ(پورٹ.cell, پورٹ.پورٹ, پورٹ.وتھ + پورٹ.چوڑائی, chunk.ناپ());
				إذا (!chunk.مضرب_2().گنتی(اگلا))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				خودکار ثابت &marker = single.marker();
				DriveChunk اگلا = DriveChunkMarker(marker.marker, marker.وتھ + marker.چوڑائی, chunk.ناپ());
				إذا (!chunk.مضرب_2().گنتی(اگلا))
					أرجع خطأ;
			} اكسر;
			افتراضي:
				أرجع خطأ;
		}
	}

	أرجع سچ;
}

منطقي Drive_Chunk_مضرب::try_append(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ)
{
	إذا (!can_append(ثنائی_ہندسہ))
		أرجع خطأ;
	چوڑائی_3 += 1;
	State constant;

	لكل (Drive_ثنائی_ہندسہ ثابت &single : ثنائی_ہندسہ.مضرب_2())
		إذا (single.is_constant())
			constant = single.constant();

	لكل (DriveChunk &single : مضرب_3)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} اكسر;
			حالة DriveType::WIRE: {
				single.wire().چوڑائی += 1;
			} اكسر;
			حالة DriveType::پورٹ_2: {
				single.پورٹ().چوڑائی += 1;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().چوڑائی += 1;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	أرجع سچ;
}

منطقي Drive_Chunk_مضرب::try_append(Drive_Chunk_مضرب ثابت &chunk)
{
	إذا (!can_append(chunk))
		أرجع خطأ;
	صحيح چوڑائی = chunk.ناپ();
	چوڑائی_3 += چوڑائی;
	Const constant;

	لكل (DriveChunk ثابت &single : chunk.مضرب_2())
		إذا (single.is_constant())
			constant = single.constant();

	لكل (DriveChunk &single : مضرب_3)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
				single.constant().append(constant);
			} اكسر;
			حالة DriveType::WIRE: {
				single.wire().چوڑائی += چوڑائی;
			} اكسر;
			حالة DriveType::پورٹ_2: {
				single.پورٹ().چوڑائی += چوڑائی;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().چوڑائی += چوڑائی;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	أرجع سچ;
}

منطقي DriveChunk::can_append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ) ثابت
{
	إذا (ناپ() == 0)
		أرجع سچ;
	إذا (ثنائی_ہندسہ.type() != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			أرجع سچ;
		حالة DriveType::CONSTANT:
			أرجع سچ;
		حالة DriveType::WIRE:
			أرجع wire_.can_append(ثنائی_ہندسہ.wire());
		حالة DriveType::پورٹ_2:
			أرجع پورٹ_3.can_append(ثنائی_ہندسہ.پورٹ());
		حالة DriveType::مضرب:
			أرجع مضرب_3.can_append(ثنائی_ہندسہ.مضرب_2());
		افتراضي:
			log_abort();
	}
}

منطقي DriveChunk::try_append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	إذا (ناپ() == 0)
		*هذا = ثنائی_ہندسہ;
	إذا (ثنائی_ہندسہ.type() != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			none_ += 1;
			أرجع سچ;
		حالة DriveType::CONSTANT:
			constant_.append(RTLIL::Const(ثنائی_ہندسہ.constant()));
			أرجع سچ;
		حالة DriveType::WIRE:
			أرجع wire_.try_append(ثنائی_ہندسہ.wire());
		حالة DriveType::پورٹ_2:
			أرجع پورٹ_3.try_append(ثنائی_ہندسہ.پورٹ());
		حالة DriveType::مضرب:
			أرجع مضرب_3.try_append(ثنائی_ہندسہ.مضرب_2());
		افتراضي:
			log_abort();
	}
}


منطقي DriveChunk::try_append(DriveChunk ثابت &chunk)
{
	إذا (ناپ() == 0)
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
		حالة DriveType::پورٹ_2:
			أرجع پورٹ_3.try_append(chunk.پورٹ());
		حالة DriveType::MARKER:
			أرجع marker_.try_append(chunk.marker());
		حالة DriveType::مضرب:
			أرجع مضرب_3.try_append(chunk.مضرب_2());
	}
	log_abort();
}

فراغ DriveSpec::append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	hash_ = 0;
	إذا (!packed()) {
		بٹ_2.ڈالو_پٹھ(ثنائی_ہندسہ);
		چوڑائی_3 += 1;
		أرجع;
	}

	إذا (chunks_.خالی_تھاواں() || !chunks_.پٹھ().try_append(ثنائی_ہندسہ))
		chunks_.emplace_پٹھ(ثنائی_ہندسہ);
	چوڑائی_3 += 1;
}

فراغ DriveSpec::append(DriveChunk ثابت &chunk)
{
	hash_ = 0;
	pack();
	إذا (chunks_.خالی_تھاواں() || !chunks_.پٹھ().try_append(chunk))
		chunks_.emplace_پٹھ(chunk);
	چوڑائی_3 += chunk.ناپ();
}

فراغ DriveSpec::pack() ثابت {
	إذا (بٹ_2.خالی_تھاواں())
		أرجع;
	std::سمتیہ<Drive_ثنائی_ہندسہ> بٹ(std::move(بٹ_2));
	لكل (خودکار &ثنائی_ہندسہ : بٹ)
		إذا (chunks_.خالی_تھاواں() || !chunks_.پٹھ().try_append(ثنائی_ہندسہ))
			chunks_.emplace_پٹھ(std::move(ثنائی_ہندسہ));
}

فراغ DriveSpec::unpack() ثابت {
	إذا (chunks_.خالی_تھاواں())
		أرجع;
	لكل (خودکار &chunk : chunks_)
	{
		لكل (صحيح i = 0, چوڑائی = chunk.ناپ(); i != چوڑائی; ++i)
		{
			بٹ_2.emplace_پٹھ(chunk[i]);
		}
	}
	chunks_.clear();
}

فراغ DriveSpec::compute_چوڑائی()
{
	چوڑائی_3 = 0;
	لكل (خودکار ثابت &chunk : chunks_)
		چوڑائی_3 += chunk.ناپ();
}

فراغ DriverMap::Drive_ثنائی_ہندسہ_گراف::جوڑو_کنارا(Drive_ثنائی_ہندسہ_Id src, Drive_ثنائی_ہندسہ_Id dst)
{
	إذا (پہلا_کنارے.emplace(src, dst).پہلا->دوجا == dst)
		أرجع;
	إذا (دوجا_کنارے.emplace(src, dst).پہلا->دوجا == dst)
		أرجع;
	ہور_کنارے[src].emplace(dst);
}

DriverMap::Drive_ثنائی_ہندسہ_Id DriverMap::Drive_ثنائی_ہندسہ_گراف::نکالو_کنارا(Drive_ثنائی_ہندسہ_Id src)
{
	// TODO unused I think?
	خودکار لبھیا_ہور = ہور_کنارے.لبھو(src);
	إذا (لبھیا_ہور != ہور_کنارے.سرا()) {
		خودکار نتیجہ = لبھیا_ہور->دوجا.نکالو();
		إذا (لبھیا_ہور->دوجا.خالی_تھاواں())
			ہور_کنارے.erase(لبھیا_ہور);
		أرجع نتیجہ;
	}

	خودکار لبھیا_دوجا = دوجا_کنارے.لبھو(src);
	إذا (لبھیا_دوجا != دوجا_کنارے.سرا()) {
		خودکار نتیجہ = لبھیا_دوجا->دوجا;
		دوجا_کنارے.erase(لبھیا_دوجا);
		أرجع نتیجہ;
	}

	خودکار لبھیا_پہلا = پہلا_کنارے.لبھو(src);
	إذا (لبھیا_پہلا != پہلا_کنارے.سرا()) {
		خودکار نتیجہ = لبھیا_پہلا->دوجا;
		پہلا_کنارے.erase(لبھیا_پہلا);
		أرجع نتیجہ;
	}

	أرجع Drive_ثنائی_ہندسہ_Id();
}

فراغ DriverMap::Drive_ثنائی_ہندسہ_گراف::clear(Drive_ثنائی_ہندسہ_Id src)
{
	پہلا_کنارے.erase(src);
	دوجا_کنارے.erase(src);
	ہور_کنارے.erase(src);
}

منطقي DriverMap::Drive_ثنائی_ہندسہ_گراف::شامل_اے(Drive_ثنائی_ہندسہ_Id src)
{
	أرجع پہلا_کنارے.گنتی(src);
}

صحيح DriverMap::Drive_ثنائی_ہندسہ_گراف::گنتی(Drive_ثنائی_ہندسہ_Id src)
{
	إذا (!پہلا_کنارے.گنتی(src))
		أرجع 0;
	إذا (!دوجا_کنارے.گنتی(src))
		أرجع 1;
	خودکار لبھیا = ہور_کنارے.لبھو(src);
	إذا (لبھیا == ہور_کنارے.سرا())
		أرجع 2;
	أرجع Get_ناپ(لبھیا->دوجا) + 2;
}

DriverMap::Drive_ثنائی_ہندسہ_Id DriverMap::Drive_ثنائی_ہندسہ_گراف::at(Drive_ثنائی_ہندسہ_Id src, صحيح اشاریہ)
{
	إذا (اشاریہ == 0)
		أرجع پہلا_کنارے.at(src);
	وإلا إذا (اشاریہ == 1)
		أرجع دوجا_کنارے.at(src);
	وإلا
		أرجع *ہور_کنارے.at(src).element(اشاریہ - 2);
}


DriverMap::ثنائی_ہندسہ_منوال DriverMap::ثنائی_ہندسہ_منوال_2(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	بدل (ثنائی_ہندسہ.type())
	{
		حالة DriveType::NONE:
			أرجع ثنائی_ہندسہ_منوال::NONE;
		حالة DriveType::CONSTANT:
			// TODO how to handle Sx here?
			أرجع ثنائی_ہندسہ.constant() == State::Sz ? ثنائی_ہندسہ_منوال::NONE : ثنائی_ہندسہ_منوال::DRIVER;
		حالة DriveType::WIRE: {
			خودکار ثابت &wire = ثنائی_ہندسہ.wire();
			منطقي driver = wire.wire->پورٹ_داخلہ;
			منطقي driven = wire.wire->پورٹ_output;

			إذا (driver && !driven)
				أرجع ثنائی_ہندسہ_منوال::DRIVER;
			وإلا إذا (driven && !driver)
				أرجع ثنائی_ہندسہ_منوال::DRIVEN;
			وإلا إذا (driver && driven)
				أرجع ثنائی_ہندسہ_منوال::TRISTATE;
			وإلا
				أرجع keep_wire(ثنائی_ہندسہ.wire().wire) ? ثنائی_ہندسہ_منوال::KEEP : ثنائی_ہندسہ_منوال::NONE;
		}
		حالة DriveType::پورٹ_2: {
			خودکار ثابت &پورٹ = ثنائی_ہندسہ.پورٹ();
			منطقي driver = celltypes.cell_output(پورٹ.cell->type, پورٹ.پورٹ);
			منطقي driven = celltypes.cell_داخلہ(پورٹ.cell->type, پورٹ.پورٹ);
			إذا (driver && !driven)
				أرجع ثنائی_ہندسہ_منوال::DRIVER;
			وإلا إذا (driven && !driver)
				أرجع ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE;
			وإلا
				أرجع ثنائی_ہندسہ_منوال::TRISTATE;
		}
		حالة DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		افتراضي:
			log_abort();
	}
}

DriverMap::Drive_ثنائی_ہندسہ_Id DriverMap::id_from_drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	بدل (ثنائی_ہندسہ.type())
	{
		حالة DriveType::NONE:
			أرجع -1;
		حالة DriveType::CONSTANT:
			أرجع (صحيح)ثنائی_ہندسہ.constant();
		حالة DriveType::WIRE: {
			خودکار ثابت &wire_ثنائی_ہندسہ = ثنائی_ہندسہ.wire();
			صحيح وتھ = اگلا_وتھ;
			خودکار insertion = wire_offsets.emplace(wire_ثنائی_ہندسہ.wire, وتھ);
			إذا (insertion.دوجا) {
				إذا (wire_ثنائی_ہندسہ.wire->چوڑائی == 1) {
					log_assert(wire_ثنائی_ہندسہ.وتھ == 0);
					isolated_drive_بٹ.emplace(وتھ, ثنائی_ہندسہ);
				} وإلا
					drive_بٹ.emplace(وتھ, Drive_ثنائی_ہندسہ_Wire(wire_ثنائی_ہندسہ.wire, 0));
				اگلا_وتھ += wire_ثنائی_ہندسہ.wire->چوڑائی;
			}
			أرجع insertion.پہلا->دوجا.id + wire_ثنائی_ہندسہ.وتھ;
		}
		حالة DriveType::پورٹ_2: {
			خودکار ثابت &پورٹ_ثنائی_ہندسہ = ثنائی_ہندسہ.پورٹ();
			خودکار کلید = std::make_pair(پورٹ_ثنائی_ہندسہ.cell, پورٹ_ثنائی_ہندسہ.پورٹ);
			صحيح وتھ = اگلا_وتھ;
			خودکار insertion = پورٹ_offsets.emplace(کلید, وتھ);
			إذا (insertion.دوجا) {
				صحيح چوڑائی = پورٹ_ثنائی_ہندسہ.cell->connections().at(پورٹ_ثنائی_ہندسہ.پورٹ).ناپ();
				إذا (چوڑائی == 1 && وتھ == 0) {
					log_assert(پورٹ_ثنائی_ہندسہ.وتھ == 0);
					isolated_drive_بٹ.emplace(وتھ, ثنائی_ہندسہ);
				} وإلا
					drive_بٹ.emplace(وتھ, Drive_ثنائی_ہندسہ_پورٹ(پورٹ_ثنائی_ہندسہ.cell, پورٹ_ثنائی_ہندسہ.پورٹ, 0));
				اگلا_وتھ += چوڑائی;
			}
			أرجع insertion.پہلا->دوجا.id + پورٹ_ثنائی_ہندسہ.وتھ;
		}
		افتراضي:
			log_assert(خطأ && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

Drive_ثنائی_ہندسہ DriverMap::drive_ثنائی_ہندسہ_from_id(Drive_ثنائی_ہندسہ_Id id)
{
	خودکار لبھیا_isolated = isolated_drive_بٹ.لبھو(id);
	إذا (لبھیا_isolated != isolated_drive_بٹ.سرا())
		أرجع لبھیا_isolated->دوجا;

	خودکار لبھیا = drive_بٹ.اتلا_bound(id);
	إذا (لبھیا == drive_بٹ.begin()) {
		أرجع id < 0 ? Drive_ثنائی_ہندسہ() : Drive_ثنائی_ہندسہ((State) id.id);
	}
	--لبھیا;
	Drive_ثنائی_ہندسہ نتیجہ = لبھیا->دوجا;
	إذا (نتیجہ.is_wire()) {
		نتیجہ.wire().وتھ += id.id - لبھیا->پہلا.id;
	} وإلا {
		log_assert(نتیجہ.is_پورٹ());
		نتیجہ.پورٹ().وتھ += id.id - لبھیا->پہلا.id;
	}
	أرجع نتیجہ;
}

فراغ DriverMap::connect_directed_ملاؤ(Drive_ثنائی_ہندسہ_Id driven_id, Drive_ثنائی_ہندسہ_Id driver_id)
{
	إذا (driven_id == driver_id)
		أرجع;

	same_driver.ملاؤ(driven_id, driver_id);

	لكل (صحيح i = 0, سرا = connected_drivers.گنتی(driven_id); i != سرا; ++i)
		connected_drivers.جوڑو_کنارا(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	لكل (صحيح i = 0, سرا = connected_undirected.گنتی(driven_id); i != سرا; ++i)
		connected_undirected.جوڑو_کنارا(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

فراغ DriverMap::connect_directed_بفر(Drive_ثنائی_ہندسہ_Id driven_id, Drive_ثنائی_ہندسہ_Id driver_id)
{
	connected_drivers.جوڑو_کنارا(driven_id, driver_id);
}

فراغ DriverMap::connect_undirected(Drive_ثنائی_ہندسہ_Id a_id, Drive_ثنائی_ہندسہ_Id b_id)
{
	connected_undirected.جوڑو_کنارا(a_id, b_id);
	connected_undirected.جوڑو_کنارا(b_id, a_id);
}

فراغ DriverMap::جوڑو(Module *module)
{
	لكل (خودکار ثابت &conn : module->connections())
		جوڑو(conn.پہلا, conn.دوجا);

	لكل (خودکار cell : module->cells())
		لكل (خودکار ثابت &conn : cell->connections())
			جوڑو_پورٹ(cell, conn.پہلا, conn.دوجا);
}

// Add a single bit connection to the driver map.
فراغ DriverMap::جوڑو(Drive_ثنائی_ہندسہ ثابت &a, Drive_ثنائی_ہندسہ ثابت &b)
{
	Drive_ثنائی_ہندسہ_Id a_id = id_from_drive_ثنائی_ہندسہ(a);
	Drive_ثنائی_ہندسہ_Id b_id = id_from_drive_ثنائی_ہندسہ(b);

	Drive_ثنائی_ہندسہ_Id orig_a_id = a_id;
	Drive_ثنائی_ہندسہ_Id orig_b_id = b_id;

	a_id = same_driver.لبھو(a_id);
	b_id = same_driver.لبھو(b_id);

	إذا (a_id == b_id)
		أرجع;

	ثنائی_ہندسہ_منوال a_منوال = ثنائی_ہندسہ_منوال_2(orig_a_id == a_id ? a : drive_ثنائی_ہندسہ_from_id(a_id));
	ثنائی_ہندسہ_منوال b_منوال = ثنائی_ہندسہ_منوال_2(orig_b_id == b_id ? b : drive_ثنائی_ہندسہ_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	إذا (a_منوال == ثنائی_ہندسہ_منوال::NONE && !(b_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE || b_منوال == ثنائی_ہندسہ_منوال::DRIVEN))
		connect_directed_ملاؤ(a_id, b_id);
	وإلا إذا (b_منوال == ثنائی_ہندسہ_منوال::NONE && !(a_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE || a_منوال == ثنائی_ہندسہ_منوال::DRIVEN))
		connect_directed_ملاؤ(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	وإلا إذا (a_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE && !(b_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE || b_منوال == ثنائی_ہندسہ_منوال::DRIVEN))
		connect_directed_بفر(a_id, b_id);
	وإلا إذا (b_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE && !(a_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE || a_منوال == ثنائی_ہندسہ_منوال::DRIVEN))
		connect_directed_بفر(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	وإلا إذا (a_منوال == ثنائی_ہندسہ_منوال::DRIVER)
		connect_directed_بفر(b_id, a_id);
	وإلا إذا (b_منوال == ثنائی_ہندسہ_منوال::DRIVER)
		connect_directed_بفر(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	وإلا
		connect_undirected(a_id, b_id);

	أرجع;
}

// Specialized version that avoids unpacking
فراغ DriverMap::جوڑو(SigSpec ثابت &a, SigSpec ثابت &b)
{
	log_assert(a.ناپ() == b.ناپ());
	خودکار ثابت &a_chunks = a.chunks();
	خودکار ثابت &b_chunks = b.chunks();

	خودکار a_chunk = a_chunks.begin();
	خودکار a_سرا = a_chunks.سرا();
	صحيح a_وتھ = 0;

	خودکار b_chunk = b_chunks.begin();
	صحيح b_وتھ = 0;

	SigChunk tmp_a, tmp_b;
	طالما (a_chunk != a_سرا) {
		صحيح a_چوڑائی = a_chunk->چوڑائی - a_وتھ;
		إذا (a_چوڑائی == 0) {
			a_وتھ = 0;
			++a_chunk;
			استمر;
		}
		صحيح b_چوڑائی = b_chunk->چوڑائی - b_وتھ;
		إذا (b_چوڑائی == 0) {
			b_وتھ = 0;
			++b_chunk;
			استمر;
		}
		صحيح چوڑائی = std::min(a_چوڑائی, b_چوڑائی);
		log_assert(چوڑائی > 0);

		SigChunk ثابت &a_subchunk =
			a_وتھ == 0 && a_چوڑائی == چوڑائی ? *a_chunk : a_chunk->کڈھو(a_وتھ, چوڑائی);
		SigChunk ثابت &b_subchunk =
			b_وتھ == 0 && b_چوڑائی == چوڑائی ? *b_chunk : b_chunk->کڈھو(b_وتھ, چوڑائی);

		جوڑو(a_subchunk, b_subchunk);

		a_وتھ += چوڑائی;
		b_وتھ += چوڑائی;
	}
}

فراغ DriverMap::جوڑو_پورٹ(Cell *cell, IdString پورٹ, SigSpec ثابت &b)
{
	صحيح وتھ = 0;
	لكل (خودکار ثابت &chunk : b.chunks()) {
		جوڑو(chunk, Drive_Chunk_پورٹ(cell, پورٹ, وتھ, chunk.چوڑائی));
		وتھ += chunk.ناپ();
	}
}

فراغ DriverMap::orient_undirected(Drive_ثنائی_ہندسہ_Id id)
{
	pool<Drive_ثنائی_ہندسہ_Id> &تکیا = orient_undirected_تکیا;
	pool<Drive_ثنائی_ہندسہ_Id> &drivers = orient_undirected_drivers;
	dict<Drive_ثنائی_ہندسہ_Id, صحيح> &فاصلہ = orient_undirected_فاصلہ;
	تکیا.clear();
	drivers.clear();

	تکیا.emplace(id);

	لكل (صحيح pos = 0; pos < Get_ناپ(تکیا); ++pos) {
		Drive_ثنائی_ہندسہ_Id current = *تکیا.element(تکیا.ناپ() - 1 - pos);
		Drive_ثنائی_ہندسہ ثنائی_ہندسہ = drive_ثنائی_ہندسہ_from_id(current);

		ثنائی_ہندسہ_منوال منوال = ثنائی_ہندسہ_منوال_2(ثنائی_ہندسہ);

		إذا (منوال == ثنائی_ہندسہ_منوال::DRIVER || منوال == ثنائی_ہندسہ_منوال::TRISTATE)
			drivers.emplace(current);

		إذا (connected_drivers.شامل_اے(current))
			drivers.emplace(current);

		صحيح undirected_driver_گنتی = connected_undirected.گنتی(current);

		لكل (صحيح i = 0; i != undirected_driver_گنتی; ++i)
			تکیا.emplace(same_driver.لبھو(connected_undirected.at(current, i)));
	}

	إذا (drivers.خالی_تھاواں())
		لكل (خودکار تکیا_id : تکیا)
			drivers.emplace(تکیا_id);

	لكل (خودکار driver : drivers)
	{
		فاصلہ.clear();
		فاصلہ.emplace(driver, 0);

		لكل (صحيح pos = 0; pos < Get_ناپ(فاصلہ); ++pos) {
			خودکار current_it = فاصلہ.element(فاصلہ.ناپ() - 1 - pos);

			Drive_ثنائی_ہندسہ_Id current = current_it->پہلا;
			صحيح undirected_driver_گنتی = connected_undirected.گنتی(current);

			لكل (صحيح i = 0; i != undirected_driver_گنتی; ++i)
			{
				Drive_ثنائی_ہندسہ_Id اگلا = same_driver.لبھو(connected_undirected.at(current, i));
				خودکار emplaced = فاصلہ.emplace(اگلا, current_it->دوجا + 1);
				إذا (emplaced.پہلا->دوجا == current_it->دوجا + 1)
					connected_oriented.جوڑو_کنارا(اگلا, current);
			}
		}
	}

	لكل (خودکار تکیا_id : تکیا)
		oriented_present.emplace(تکیا_id);
}

Drive_ثنائی_ہندسہ DriverMap::عامل()(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	إذا (ثنائی_ہندسہ.type() == DriveType::MARKER || ثنائی_ہندسہ.type() == DriveType::NONE)
		أرجع ثنائی_ہندسہ;
	إذا (ثنائی_ہندسہ.type() == DriveType::مضرب)
	{
		Drive_ثنائی_ہندسہ نتیجہ;
		لكل (خودکار ثابت &inner : ثنائی_ہندسہ.مضرب_2().مضرب_2())
			نتیجہ.ملاؤ((*هذا)(inner));
		أرجع نتیجہ;
	}

	Drive_ثنائی_ہندسہ_Id ثنائی_ہندسہ_id = id_from_drive_ثنائی_ہندسہ(ثنائی_ہندسہ);

	Drive_ثنائی_ہندسہ_Id ثنائی_ہندسہ_repr_id = same_driver.لبھو(ثنائی_ہندسہ_id);

	Drive_ثنائی_ہندسہ ثنائی_ہندسہ_repr = drive_ثنائی_ہندسہ_from_id(ثنائی_ہندسہ_repr_id);

	ثنائی_ہندسہ_منوال منوال = ثنائی_ہندسہ_منوال_2(ثنائی_ہندسہ_repr);

	إذا (منوال == ثنائی_ہندسہ_منوال::KEEP && ثنائی_ہندسہ_repr_id != ثنائی_ہندسہ_id)
		أرجع ثنائی_ہندسہ_repr;

	صحيح implicit_driver_گنتی = connected_drivers.گنتی(ثنائی_ہندسہ_repr_id);
	إذا (connected_undirected.شامل_اے(ثنائی_ہندسہ_repr_id) && !oriented_present.گنتی(ثنائی_ہندسہ_repr_id))
		orient_undirected(ثنائی_ہندسہ_repr_id);

	Drive_ثنائی_ہندسہ driver;

	إذا (منوال == ثنائی_ہندسہ_منوال::DRIVER || منوال == ثنائی_ہندسہ_منوال::TRISTATE)
		driver = ثنائی_ہندسہ_repr;

	لكل (صحيح i = 0; i != implicit_driver_گنتی; ++i)
		driver.ملاؤ(drive_ثنائی_ہندسہ_from_id(connected_drivers.at(ثنائی_ہندسہ_repr_id, i)));

	صحيح oriented_driver_گنتی = connected_oriented.گنتی(ثنائی_ہندسہ_repr_id);
	لكل (صحيح i = 0; i != oriented_driver_گنتی; ++i)
		driver.ملاؤ(drive_ثنائی_ہندسہ_from_id(connected_oriented.at(ثنائی_ہندسہ_repr_id, i)));

	أرجع driver;
}

DriveSpec DriverMap::عامل()(DriveSpec spec)
{
	DriveSpec نتیجہ;

	لكل (صحيح i = 0, چوڑائی = spec.ناپ(); i != چوڑائی; ++i)
		نتیجہ.append((*هذا)(spec[i]));

	أرجع نتیجہ;
}

std::string log_signal(DriveChunkWire ثابت &chunk)
{
	std::string id = chunk.wire->name.unescape();
	إذا (chunk.is_سارا())
		أرجع id;
	إذا (chunk.چوڑائی == 1)
		أرجع stringf("%s [%d]", id, chunk.وتھ);
	أرجع stringf("%s [%d:%d]", id, chunk.وتھ + chunk.چوڑائی - 1, chunk.وتھ);
}


std::string log_signal(Drive_Chunk_پورٹ ثابت &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string پورٹ_id = chunk.cell->twines().unescaped_str(chunk.پورٹ);
	إذا (chunk.is_سارا())
		أرجع stringf("%s <%s>", cell_id, پورٹ_id);
	إذا (chunk.چوڑائی == 1)
		أرجع stringf("%s <%s> [%d]", cell_id, پورٹ_id, chunk.وتھ);
	أرجع stringf("%s <%s> [%d:%d]", cell_id, پورٹ_id, chunk.وتھ + chunk.چوڑائی - 1, chunk.وتھ);
}

std::string log_signal(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.چوڑائی == 1)
		أرجع stringf("<marker %d> [%d]", chunk.marker, chunk.وتھ);
	أرجع stringf("<marker %d> [%d:%d]", chunk.marker, chunk.وتھ + chunk.چوڑائی - 1, chunk.وتھ);
}

std::string log_signal(DriveChunk ثابت &chunk)
{
	بدل (chunk.type())
	{
		حالة DriveType::NONE:
			أرجع stringf("<none x%d>", chunk.ناپ());
		حالة DriveType::CONSTANT:
			أرجع log_const(chunk.constant());
		حالة DriveType::WIRE:
			أرجع log_signal(chunk.wire());
		حالة DriveType::پورٹ_2:
			أرجع log_signal(chunk.پورٹ());
		حالة DriveType::MARKER:
			أرجع log_signal(chunk.marker());
		حالة DriveType::مضرب: {
			std::string str = "<multiple";
			ثابت حرف *sep = " ";
			لكل (خودکار ثابت &single : chunk.مضرب_2().مضرب_2()) {
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
	خودکار &chunks = spec.chunks();
	إذا (chunks.خالی_تھاواں())
		أرجع "{}";
	إذا (chunks.ناپ() == 1)
		أرجع log_signal(chunks[0]);

	std::string str;
	ثابت حرف *sep = "{ ";

	لكل (خودکار i = chunks.rbegin(), سرا = chunks.rend(); i != سرا; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	أرجع str;
}

YOSYS_NAMESPACE_سرا
