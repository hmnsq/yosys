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

Drive_بټ::Drive_بټ(Sig_بټ ثابت &بټ)
{
	إذا (بټ.is_wire())
		*هذا = Drive_بټ_Wire(بټ.wire, بټ.دننه_واټن);
	وإلا
		*هذا = بټ.معلومات;
}

فراغ Drive_بټ::یوځای(Drive_بټ ثابت &other)
{
	إذا (other.type_ == DriveType::NONE)
		أرجع;
	إذا (type_ == DriveType::NONE) {
		*هذا = other;
		أرجع;
	}
	إذا (type_ != DriveType::مضرب) {
		Drive_بټ_مضرب multi(std::move(*هذا));
		*هذا = std::move(multi);
	}
	مضرب_2().یوځای(other);
}


فراغ Drive_بټ_مضرب::یوځای(Drive_بټ ثابت &single)
{
	إذا (single.type() == DriveType::NONE)
		أرجع;
	إذا (single.type() == DriveType::مضرب) {
		یوځای(single.مضرب_2());
		أرجع;
	}
	مضرب_3.emplace(single);
}

فراغ Drive_بټ_مضرب::یوځای(Drive_بټ &&single)
{
	إذا (single.type() == DriveType::NONE)
		أرجع;
	إذا (single.type() == DriveType::مضرب) {
		یوځای(std::move(single.مضرب_2()));
		أرجع;
	}
	مضرب_3.emplace(std::move(single));
}

Drive_بټ_مضرب Drive_Chunk_مضرب::چلوونکی[](صحيح i) ثابت
{
	Drive_بټ_مضرب پایله;
	لكل (خپلکاری ثابت &single : مضرب_3)
		پایله.یوځای(single[i]);
	أرجع پایله;
}


منطقي DriveChunkWire::can_append(Drive_بټ_Wire ثابت &بټ) ثابت
{
	أرجع بټ.wire == wire && بټ.دننه_واټن == دننه_واټن + پلنوالی;
}

منطقي DriveChunkWire::try_append(Drive_بټ_Wire ثابت &بټ)
{
	إذا (!can_append(بټ))
		أرجع خطأ;
	پلنوالی += 1;
	أرجع ریښتیا;
}

منطقي DriveChunkWire::try_append(DriveChunkWire ثابت &chunk)
{
	إذا (chunk.wire != wire || chunk.دننه_واټن != دننه_واټن + پلنوالی)
		أرجع خطأ;
	پلنوالی += chunk.پلنوالی;
	أرجع ریښتیا;
}

منطقي Drive_Chunk_پورټ::can_append(Drive_بټ_پورټ ثابت &بټ) ثابت
{
	أرجع بټ.cell == cell && بټ.پورټ == پورټ && بټ.دننه_واټن == دننه_واټن + پلنوالی;
}

منطقي Drive_Chunk_پورټ::try_append(Drive_بټ_پورټ ثابت &بټ)
{
	إذا (!can_append(بټ))
		أرجع خطأ;
	پلنوالی += 1;
	أرجع ریښتیا;
}

منطقي Drive_Chunk_پورټ::try_append(Drive_Chunk_پورټ ثابت &chunk)
{
	إذا (chunk.cell != cell || chunk.پورټ != پورټ || chunk.دننه_واټن != دننه_واټن + پلنوالی)
		أرجع خطأ;
	پلنوالی += chunk.پلنوالی;
	أرجع ریښتیا;
}

منطقي DriveChunkMarker::can_append(Drive_بټ_Marker ثابت &بټ) ثابت
{
	أرجع بټ.marker == marker && بټ.دننه_واټن == دننه_واټن + پلنوالی;
}

منطقي DriveChunkMarker::try_append(Drive_بټ_Marker ثابت &بټ)
{
	إذا (!can_append(بټ))
		أرجع خطأ;
	پلنوالی += 1;
	أرجع ریښتیا;
}

منطقي DriveChunkMarker::try_append(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.marker != marker || chunk.دننه_واټن != دننه_واټن + پلنوالی)
		أرجع خطأ;
	پلنوالی += chunk.پلنوالی;
	أرجع ریښتیا;
}


منطقي Drive_Chunk_مضرب::can_append(Drive_بټ_مضرب ثابت &بټ) ثابت
{
	إذا (بټ.مضرب_2().اندازه() != مضرب_3.اندازه())
		أرجع خطأ;

	صحيح const_drivers = 0;
	لكل (DriveChunk ثابت &single : مضرب_3)
		إذا (single.is_constant())
			const_drivers += 1;

	إذا (const_drivers > 1)
		أرجع خطأ;

	لكل (Drive_بټ ثابت &single : بټ.مضرب_2())
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
				خپلکاری ثابت &wire = single.wire();
				Drive_بټ بل = Drive_بټ_Wire(wire.wire, wire.دننه_واټن + wire.پلنوالی);
				إذا (!بټ.مضرب_2().شمېر(بل))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::پورټ_2: {
				خپلکاری ثابت &پورټ = single.پورټ();
				Drive_بټ بل = Drive_بټ_پورټ(پورټ.cell, پورټ.پورټ, پورټ.دننه_واټن + پورټ.پلنوالی);
				إذا (!بټ.مضرب_2().شمېر(بل))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				خپلکاری ثابت &marker = single.marker();
				Drive_بټ بل = Drive_بټ_Marker(marker.marker, marker.دننه_واټن + marker.پلنوالی);
				إذا (!بټ.مضرب_2().شمېر(بل))
					أرجع خطأ;
			} اكسر;
			افتراضي:
				أرجع خطأ;
		}
	}

	أرجع ریښتیا;
}

منطقي Drive_Chunk_مضرب::can_append(Drive_Chunk_مضرب ثابت &chunk) ثابت
{
	إذا (chunk.مضرب_2().اندازه() != مضرب_3.اندازه())
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
				خپلکاری ثابت &wire = single.wire();
				DriveChunk بل = DriveChunkWire(wire.wire, wire.دننه_واټن + wire.پلنوالی, chunk.اندازه());
				إذا (!chunk.مضرب_2().شمېر(بل))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::پورټ_2: {
				خپلکاری ثابت &پورټ = single.پورټ();
				DriveChunk بل = Drive_Chunk_پورټ(پورټ.cell, پورټ.پورټ, پورټ.دننه_واټن + پورټ.پلنوالی, chunk.اندازه());
				إذا (!chunk.مضرب_2().شمېر(بل))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				خپلکاری ثابت &marker = single.marker();
				DriveChunk بل = DriveChunkMarker(marker.marker, marker.دننه_واټن + marker.پلنوالی, chunk.اندازه());
				إذا (!chunk.مضرب_2().شمېر(بل))
					أرجع خطأ;
			} اكسر;
			افتراضي:
				أرجع خطأ;
		}
	}

	أرجع ریښتیا;
}

منطقي Drive_Chunk_مضرب::try_append(Drive_بټ_مضرب ثابت &بټ)
{
	إذا (!can_append(بټ))
		أرجع خطأ;
	پلنوالی_3 += 1;
	State constant;

	لكل (Drive_بټ ثابت &single : بټ.مضرب_2())
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
				single.wire().پلنوالی += 1;
			} اكسر;
			حالة DriveType::پورټ_2: {
				single.پورټ().پلنوالی += 1;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().پلنوالی += 1;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	أرجع ریښتیا;
}

منطقي Drive_Chunk_مضرب::try_append(Drive_Chunk_مضرب ثابت &chunk)
{
	إذا (!can_append(chunk))
		أرجع خطأ;
	صحيح پلنوالی = chunk.اندازه();
	پلنوالی_3 += پلنوالی;
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
				single.wire().پلنوالی += پلنوالی;
			} اكسر;
			حالة DriveType::پورټ_2: {
				single.پورټ().پلنوالی += پلنوالی;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().پلنوالی += پلنوالی;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	أرجع ریښتیا;
}

منطقي DriveChunk::can_append(Drive_بټ ثابت &بټ) ثابت
{
	إذا (اندازه() == 0)
		أرجع ریښتیا;
	إذا (بټ.type() != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			أرجع ریښتیا;
		حالة DriveType::CONSTANT:
			أرجع ریښتیا;
		حالة DriveType::WIRE:
			أرجع wire_.can_append(بټ.wire());
		حالة DriveType::پورټ_2:
			أرجع پورټ_3.can_append(بټ.پورټ());
		حالة DriveType::مضرب:
			أرجع مضرب_3.can_append(بټ.مضرب_2());
		افتراضي:
			log_abort();
	}
}

منطقي DriveChunk::try_append(Drive_بټ ثابت &بټ)
{
	إذا (اندازه() == 0)
		*هذا = بټ;
	إذا (بټ.type() != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			none_ += 1;
			أرجع ریښتیا;
		حالة DriveType::CONSTANT:
			constant_.append(RTLIL::Const(بټ.constant()));
			أرجع ریښتیا;
		حالة DriveType::WIRE:
			أرجع wire_.try_append(بټ.wire());
		حالة DriveType::پورټ_2:
			أرجع پورټ_3.try_append(بټ.پورټ());
		حالة DriveType::مضرب:
			أرجع مضرب_3.try_append(بټ.مضرب_2());
		افتراضي:
			log_abort();
	}
}


منطقي DriveChunk::try_append(DriveChunk ثابت &chunk)
{
	إذا (اندازه() == 0)
		*هذا = chunk;
	إذا (chunk.type_ != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			none_ += chunk.none_;
			أرجع ریښتیا;
		حالة DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			أرجع ریښتیا;
		حالة DriveType::WIRE:
			أرجع wire_.try_append(chunk.wire());
		حالة DriveType::پورټ_2:
			أرجع پورټ_3.try_append(chunk.پورټ());
		حالة DriveType::MARKER:
			أرجع marker_.try_append(chunk.marker());
		حالة DriveType::مضرب:
			أرجع مضرب_3.try_append(chunk.مضرب_2());
	}
	log_abort();
}

فراغ DriveSpec::append(Drive_بټ ثابت &بټ)
{
	hash_ = 0;
	إذا (!packed()) {
		بټونه_2.ټېل_شا(بټ);
		پلنوالی_3 += 1;
		أرجع;
	}

	إذا (chunks_.تشه() || !chunks_.شا().try_append(بټ))
		chunks_.emplace_شا(بټ);
	پلنوالی_3 += 1;
}

فراغ DriveSpec::append(DriveChunk ثابت &chunk)
{
	hash_ = 0;
	pack();
	إذا (chunks_.تشه() || !chunks_.شا().try_append(chunk))
		chunks_.emplace_شا(chunk);
	پلنوالی_3 += chunk.اندازه();
}

فراغ DriveSpec::pack() ثابت {
	إذا (بټونه_2.تشه())
		أرجع;
	std::ویکټور<Drive_بټ> بټونه(std::move(بټونه_2));
	لكل (خپلکاری &بټ : بټونه)
		إذا (chunks_.تشه() || !chunks_.شا().try_append(بټ))
			chunks_.emplace_شا(std::move(بټ));
}

فراغ DriveSpec::unpack() ثابت {
	إذا (chunks_.تشه())
		أرجع;
	لكل (خپلکاری &chunk : chunks_)
	{
		لكل (صحيح i = 0, پلنوالی = chunk.اندازه(); i != پلنوالی; ++i)
		{
			بټونه_2.emplace_شا(chunk[i]);
		}
	}
	chunks_.clear();
}

فراغ DriveSpec::compute_پلنوالی()
{
	پلنوالی_3 = 0;
	لكل (خپلکاری ثابت &chunk : chunks_)
		پلنوالی_3 += chunk.اندازه();
}

فراغ DriverMap::Drive_بټ_ګراف::جمع_کړه_څنډه(Drive_بټ_Id src, Drive_بټ_Id dst)
{
	إذا (لومړی_څنډې.emplace(src, dst).لومړی->دوهم == dst)
		أرجع;
	إذا (دوهم_څنډې.emplace(src, dst).لومړی->دوهم == dst)
		أرجع;
	نور_څنډې[src].emplace(dst);
}

DriverMap::Drive_بټ_Id DriverMap::Drive_بټ_ګراف::راوباسه_څنډه(Drive_بټ_Id src)
{
	// TODO unused I think?
	خپلکاری وموندل_نور = نور_څنډې.پیدا_کړه(src);
	إذا (وموندل_نور != نور_څنډې.پای_ټکی()) {
		خپلکاری پایله = وموندل_نور->دوهم.راوباسه();
		إذا (وموندل_نور->دوهم.تشه())
			نور_څنډې.erase(وموندل_نور);
		أرجع پایله;
	}

	خپلکاری وموندل_دوهم = دوهم_څنډې.پیدا_کړه(src);
	إذا (وموندل_دوهم != دوهم_څنډې.پای_ټکی()) {
		خپلکاری پایله = وموندل_دوهم->دوهم;
		دوهم_څنډې.erase(وموندل_دوهم);
		أرجع پایله;
	}

	خپلکاری وموندل_لومړی = لومړی_څنډې.پیدا_کړه(src);
	إذا (وموندل_لومړی != لومړی_څنډې.پای_ټکی()) {
		خپلکاری پایله = وموندل_لومړی->دوهم;
		لومړی_څنډې.erase(وموندل_لومړی);
		أرجع پایله;
	}

	أرجع Drive_بټ_Id();
}

فراغ DriverMap::Drive_بټ_ګراف::clear(Drive_بټ_Id src)
{
	لومړی_څنډې.erase(src);
	دوهم_څنډې.erase(src);
	نور_څنډې.erase(src);
}

منطقي DriverMap::Drive_بټ_ګراف::لري(Drive_بټ_Id src)
{
	أرجع لومړی_څنډې.شمېر(src);
}

صحيح DriverMap::Drive_بټ_ګراف::شمېر(Drive_بټ_Id src)
{
	إذا (!لومړی_څنډې.شمېر(src))
		أرجع 0;
	إذا (!دوهم_څنډې.شمېر(src))
		أرجع 1;
	خپلکاری وموندل = نور_څنډې.پیدا_کړه(src);
	إذا (وموندل == نور_څنډې.پای_ټکی())
		أرجع 2;
	أرجع Get_اندازه(وموندل->دوهم) + 2;
}

DriverMap::Drive_بټ_Id DriverMap::Drive_بټ_ګراف::at(Drive_بټ_Id src, صحيح شاخص)
{
	إذا (شاخص == 0)
		أرجع لومړی_څنډې.at(src);
	وإلا إذا (شاخص == 1)
		أرجع دوهم_څنډې.at(src);
	وإلا
		أرجع *نور_څنډې.at(src).element(شاخص - 2);
}


DriverMap::بټ_ډېرپېښ DriverMap::بټ_ډېرپېښ_2(Drive_بټ ثابت &بټ)
{
	بدل (بټ.type())
	{
		حالة DriveType::NONE:
			أرجع بټ_ډېرپېښ::NONE;
		حالة DriveType::CONSTANT:
			// TODO how to handle Sx here?
			أرجع بټ.constant() == State::Sz ? بټ_ډېرپېښ::NONE : بټ_ډېرپېښ::DRIVER;
		حالة DriveType::WIRE: {
			خپلکاری ثابت &wire = بټ.wire();
			منطقي driver = wire.wire->پورټ_ننوت;
			منطقي driven = wire.wire->پورټ_output;

			إذا (driver && !driven)
				أرجع بټ_ډېرپېښ::DRIVER;
			وإلا إذا (driven && !driver)
				أرجع بټ_ډېرپېښ::DRIVEN;
			وإلا إذا (driver && driven)
				أرجع بټ_ډېرپېښ::TRISTATE;
			وإلا
				أرجع keep_wire(بټ.wire().wire) ? بټ_ډېرپېښ::KEEP : بټ_ډېرپېښ::NONE;
		}
		حالة DriveType::پورټ_2: {
			خپلکاری ثابت &پورټ = بټ.پورټ();
			منطقي driver = celltypes.cell_output(پورټ.cell->type, پورټ.پورټ);
			منطقي driven = celltypes.cell_ننوت(پورټ.cell->type, پورټ.پورټ);
			إذا (driver && !driven)
				أرجع بټ_ډېرپېښ::DRIVER;
			وإلا إذا (driven && !driver)
				أرجع بټ_ډېرپېښ::DRIVEN_UNIQUE;
			وإلا
				أرجع بټ_ډېرپېښ::TRISTATE;
		}
		حالة DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		افتراضي:
			log_abort();
	}
}

DriverMap::Drive_بټ_Id DriverMap::id_from_drive_بټ(Drive_بټ ثابت &بټ)
{
	بدل (بټ.type())
	{
		حالة DriveType::NONE:
			أرجع -1;
		حالة DriveType::CONSTANT:
			أرجع (صحيح)بټ.constant();
		حالة DriveType::WIRE: {
			خپلکاری ثابت &wire_بټ = بټ.wire();
			صحيح دننه_واټن = بل_دننه_واټن;
			خپلکاری insertion = wire_offsets.emplace(wire_بټ.wire, دننه_واټن);
			إذا (insertion.دوهم) {
				إذا (wire_بټ.wire->پلنوالی == 1) {
					log_assert(wire_بټ.دننه_واټن == 0);
					isolated_drive_بټونه.emplace(دننه_واټن, بټ);
				} وإلا
					drive_بټونه.emplace(دننه_واټن, Drive_بټ_Wire(wire_بټ.wire, 0));
				بل_دننه_واټن += wire_بټ.wire->پلنوالی;
			}
			أرجع insertion.لومړی->دوهم.id + wire_بټ.دننه_واټن;
		}
		حالة DriveType::پورټ_2: {
			خپلکاری ثابت &پورټ_بټ = بټ.پورټ();
			خپلکاری کلی = std::make_pair(پورټ_بټ.cell, پورټ_بټ.پورټ);
			صحيح دننه_واټن = بل_دننه_واټن;
			خپلکاری insertion = پورټ_offsets.emplace(کلی, دننه_واټن);
			إذا (insertion.دوهم) {
				صحيح پلنوالی = پورټ_بټ.cell->connections().at(پورټ_بټ.پورټ).اندازه();
				إذا (پلنوالی == 1 && دننه_واټن == 0) {
					log_assert(پورټ_بټ.دننه_واټن == 0);
					isolated_drive_بټونه.emplace(دننه_واټن, بټ);
				} وإلا
					drive_بټونه.emplace(دننه_واټن, Drive_بټ_پورټ(پورټ_بټ.cell, پورټ_بټ.پورټ, 0));
				بل_دننه_واټن += پلنوالی;
			}
			أرجع insertion.لومړی->دوهم.id + پورټ_بټ.دننه_واټن;
		}
		افتراضي:
			log_assert(خطأ && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

Drive_بټ DriverMap::drive_بټ_from_id(Drive_بټ_Id id)
{
	خپلکاری وموندل_isolated = isolated_drive_بټونه.پیدا_کړه(id);
	إذا (وموندل_isolated != isolated_drive_بټونه.پای_ټکی())
		أرجع وموندل_isolated->دوهم;

	خپلکاری وموندل = drive_بټونه.پاسنی_bound(id);
	إذا (وموندل == drive_بټونه.begin()) {
		أرجع id < 0 ? Drive_بټ() : Drive_بټ((State) id.id);
	}
	--وموندل;
	Drive_بټ پایله = وموندل->دوهم;
	إذا (پایله.is_wire()) {
		پایله.wire().دننه_واټن += id.id - وموندل->لومړی.id;
	} وإلا {
		log_assert(پایله.is_پورټ());
		پایله.پورټ().دننه_واټن += id.id - وموندل->لومړی.id;
	}
	أرجع پایله;
}

فراغ DriverMap::connect_directed_یوځای(Drive_بټ_Id driven_id, Drive_بټ_Id driver_id)
{
	إذا (driven_id == driver_id)
		أرجع;

	same_driver.یوځای(driven_id, driver_id);

	لكل (صحيح i = 0, پای_ټکی = connected_drivers.شمېر(driven_id); i != پای_ټکی; ++i)
		connected_drivers.جمع_کړه_څنډه(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	لكل (صحيح i = 0, پای_ټکی = connected_undirected.شمېر(driven_id); i != پای_ټکی; ++i)
		connected_undirected.جمع_کړه_څنډه(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

فراغ DriverMap::connect_directed_بفر(Drive_بټ_Id driven_id, Drive_بټ_Id driver_id)
{
	connected_drivers.جمع_کړه_څنډه(driven_id, driver_id);
}

فراغ DriverMap::connect_undirected(Drive_بټ_Id a_id, Drive_بټ_Id b_id)
{
	connected_undirected.جمع_کړه_څنډه(a_id, b_id);
	connected_undirected.جمع_کړه_څنډه(b_id, a_id);
}

فراغ DriverMap::جمع_کړه(Module *module)
{
	لكل (خپلکاری ثابت &conn : module->connections())
		جمع_کړه(conn.لومړی, conn.دوهم);

	لكل (خپلکاری cell : module->cells())
		لكل (خپلکاری ثابت &conn : cell->connections())
			جمع_کړه_پورټ(cell, conn.لومړی, conn.دوهم);
}

// Add a single bit connection to the driver map.
فراغ DriverMap::جمع_کړه(Drive_بټ ثابت &a, Drive_بټ ثابت &b)
{
	Drive_بټ_Id a_id = id_from_drive_بټ(a);
	Drive_بټ_Id b_id = id_from_drive_بټ(b);

	Drive_بټ_Id orig_a_id = a_id;
	Drive_بټ_Id orig_b_id = b_id;

	a_id = same_driver.پیدا_کړه(a_id);
	b_id = same_driver.پیدا_کړه(b_id);

	إذا (a_id == b_id)
		أرجع;

	بټ_ډېرپېښ a_ډېرپېښ = بټ_ډېرپېښ_2(orig_a_id == a_id ? a : drive_بټ_from_id(a_id));
	بټ_ډېرپېښ b_ډېرپېښ = بټ_ډېرپېښ_2(orig_b_id == b_id ? b : drive_بټ_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	إذا (a_ډېرپېښ == بټ_ډېرپېښ::NONE && !(b_ډېرپېښ == بټ_ډېرپېښ::DRIVEN_UNIQUE || b_ډېرپېښ == بټ_ډېرپېښ::DRIVEN))
		connect_directed_یوځای(a_id, b_id);
	وإلا إذا (b_ډېرپېښ == بټ_ډېرپېښ::NONE && !(a_ډېرپېښ == بټ_ډېرپېښ::DRIVEN_UNIQUE || a_ډېرپېښ == بټ_ډېرپېښ::DRIVEN))
		connect_directed_یوځای(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	وإلا إذا (a_ډېرپېښ == بټ_ډېرپېښ::DRIVEN_UNIQUE && !(b_ډېرپېښ == بټ_ډېرپېښ::DRIVEN_UNIQUE || b_ډېرپېښ == بټ_ډېرپېښ::DRIVEN))
		connect_directed_بفر(a_id, b_id);
	وإلا إذا (b_ډېرپېښ == بټ_ډېرپېښ::DRIVEN_UNIQUE && !(a_ډېرپېښ == بټ_ډېرپېښ::DRIVEN_UNIQUE || a_ډېرپېښ == بټ_ډېرپېښ::DRIVEN))
		connect_directed_بفر(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	وإلا إذا (a_ډېرپېښ == بټ_ډېرپېښ::DRIVER)
		connect_directed_بفر(b_id, a_id);
	وإلا إذا (b_ډېرپېښ == بټ_ډېرپېښ::DRIVER)
		connect_directed_بفر(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	وإلا
		connect_undirected(a_id, b_id);

	أرجع;
}

// Specialized version that avoids unpacking
فراغ DriverMap::جمع_کړه(SigSpec ثابت &a, SigSpec ثابت &b)
{
	log_assert(a.اندازه() == b.اندازه());
	خپلکاری ثابت &a_chunks = a.chunks();
	خپلکاری ثابت &b_chunks = b.chunks();

	خپلکاری a_chunk = a_chunks.begin();
	خپلکاری a_پای_ټکی = a_chunks.پای_ټکی();
	صحيح a_دننه_واټن = 0;

	خپلکاری b_chunk = b_chunks.begin();
	صحيح b_دننه_واټن = 0;

	SigChunk tmp_a, tmp_b;
	طالما (a_chunk != a_پای_ټکی) {
		صحيح a_پلنوالی = a_chunk->پلنوالی - a_دننه_واټن;
		إذا (a_پلنوالی == 0) {
			a_دننه_واټن = 0;
			++a_chunk;
			استمر;
		}
		صحيح b_پلنوالی = b_chunk->پلنوالی - b_دننه_واټن;
		إذا (b_پلنوالی == 0) {
			b_دننه_واټن = 0;
			++b_chunk;
			استمر;
		}
		صحيح پلنوالی = std::min(a_پلنوالی, b_پلنوالی);
		log_assert(پلنوالی > 0);

		SigChunk ثابت &a_subchunk =
			a_دننه_واټن == 0 && a_پلنوالی == پلنوالی ? *a_chunk : a_chunk->ایستل(a_دننه_واټن, پلنوالی);
		SigChunk ثابت &b_subchunk =
			b_دننه_واټن == 0 && b_پلنوالی == پلنوالی ? *b_chunk : b_chunk->ایستل(b_دننه_واټن, پلنوالی);

		جمع_کړه(a_subchunk, b_subchunk);

		a_دننه_واټن += پلنوالی;
		b_دننه_واټن += پلنوالی;
	}
}

فراغ DriverMap::جمع_کړه_پورټ(Cell *cell, IdString پورټ, SigSpec ثابت &b)
{
	صحيح دننه_واټن = 0;
	لكل (خپلکاری ثابت &chunk : b.chunks()) {
		جمع_کړه(chunk, Drive_Chunk_پورټ(cell, پورټ, دننه_واټن, chunk.پلنوالی));
		دننه_واټن += chunk.اندازه();
	}
}

فراغ DriverMap::orient_undirected(Drive_بټ_Id id)
{
	pool<Drive_بټ_Id> &کتل_شوی = orient_undirected_کتل_شوی;
	pool<Drive_بټ_Id> &drivers = orient_undirected_drivers;
	dict<Drive_بټ_Id, صحيح> &واټن = orient_undirected_واټن;
	کتل_شوی.clear();
	drivers.clear();

	کتل_شوی.emplace(id);

	لكل (صحيح pos = 0; pos < Get_اندازه(کتل_شوی); ++pos) {
		Drive_بټ_Id current = *کتل_شوی.element(کتل_شوی.اندازه() - 1 - pos);
		Drive_بټ بټ = drive_بټ_from_id(current);

		بټ_ډېرپېښ ډېرپېښ = بټ_ډېرپېښ_2(بټ);

		إذا (ډېرپېښ == بټ_ډېرپېښ::DRIVER || ډېرپېښ == بټ_ډېرپېښ::TRISTATE)
			drivers.emplace(current);

		إذا (connected_drivers.لري(current))
			drivers.emplace(current);

		صحيح undirected_driver_شمېر = connected_undirected.شمېر(current);

		لكل (صحيح i = 0; i != undirected_driver_شمېر; ++i)
			کتل_شوی.emplace(same_driver.پیدا_کړه(connected_undirected.at(current, i)));
	}

	إذا (drivers.تشه())
		لكل (خپلکاری کتل_شوی_id : کتل_شوی)
			drivers.emplace(کتل_شوی_id);

	لكل (خپلکاری driver : drivers)
	{
		واټن.clear();
		واټن.emplace(driver, 0);

		لكل (صحيح pos = 0; pos < Get_اندازه(واټن); ++pos) {
			خپلکاری current_it = واټن.element(واټن.اندازه() - 1 - pos);

			Drive_بټ_Id current = current_it->لومړی;
			صحيح undirected_driver_شمېر = connected_undirected.شمېر(current);

			لكل (صحيح i = 0; i != undirected_driver_شمېر; ++i)
			{
				Drive_بټ_Id بل = same_driver.پیدا_کړه(connected_undirected.at(current, i));
				خپلکاری emplaced = واټن.emplace(بل, current_it->دوهم + 1);
				إذا (emplaced.لومړی->دوهم == current_it->دوهم + 1)
					connected_oriented.جمع_کړه_څنډه(بل, current);
			}
		}
	}

	لكل (خپلکاری کتل_شوی_id : کتل_شوی)
		oriented_present.emplace(کتل_شوی_id);
}

Drive_بټ DriverMap::چلوونکی()(Drive_بټ ثابت &بټ)
{
	إذا (بټ.type() == DriveType::MARKER || بټ.type() == DriveType::NONE)
		أرجع بټ;
	إذا (بټ.type() == DriveType::مضرب)
	{
		Drive_بټ پایله;
		لكل (خپلکاری ثابت &inner : بټ.مضرب_2().مضرب_2())
			پایله.یوځای((*هذا)(inner));
		أرجع پایله;
	}

	Drive_بټ_Id بټ_id = id_from_drive_بټ(بټ);

	Drive_بټ_Id بټ_repr_id = same_driver.پیدا_کړه(بټ_id);

	Drive_بټ بټ_repr = drive_بټ_from_id(بټ_repr_id);

	بټ_ډېرپېښ ډېرپېښ = بټ_ډېرپېښ_2(بټ_repr);

	إذا (ډېرپېښ == بټ_ډېرپېښ::KEEP && بټ_repr_id != بټ_id)
		أرجع بټ_repr;

	صحيح implicit_driver_شمېر = connected_drivers.شمېر(بټ_repr_id);
	إذا (connected_undirected.لري(بټ_repr_id) && !oriented_present.شمېر(بټ_repr_id))
		orient_undirected(بټ_repr_id);

	Drive_بټ driver;

	إذا (ډېرپېښ == بټ_ډېرپېښ::DRIVER || ډېرپېښ == بټ_ډېرپېښ::TRISTATE)
		driver = بټ_repr;

	لكل (صحيح i = 0; i != implicit_driver_شمېر; ++i)
		driver.یوځای(drive_بټ_from_id(connected_drivers.at(بټ_repr_id, i)));

	صحيح oriented_driver_شمېر = connected_oriented.شمېر(بټ_repr_id);
	لكل (صحيح i = 0; i != oriented_driver_شمېر; ++i)
		driver.یوځای(drive_بټ_from_id(connected_oriented.at(بټ_repr_id, i)));

	أرجع driver;
}

DriveSpec DriverMap::چلوونکی()(DriveSpec spec)
{
	DriveSpec پایله;

	لكل (صحيح i = 0, پلنوالی = spec.اندازه(); i != پلنوالی; ++i)
		پایله.append((*هذا)(spec[i]));

	أرجع پایله;
}

std::string log_signal(DriveChunkWire ثابت &chunk)
{
	std::string id = chunk.wire->name.unescape();
	إذا (chunk.is_ټولیز())
		أرجع id;
	إذا (chunk.پلنوالی == 1)
		أرجع stringf("%s [%d]", id, chunk.دننه_واټن);
	أرجع stringf("%s [%d:%d]", id, chunk.دننه_واټن + chunk.پلنوالی - 1, chunk.دننه_واټن);
}


std::string log_signal(Drive_Chunk_پورټ ثابت &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string پورټ_id = chunk.cell->twines().unescaped_str(chunk.پورټ);
	إذا (chunk.is_ټولیز())
		أرجع stringf("%s <%s>", cell_id, پورټ_id);
	إذا (chunk.پلنوالی == 1)
		أرجع stringf("%s <%s> [%d]", cell_id, پورټ_id, chunk.دننه_واټن);
	أرجع stringf("%s <%s> [%d:%d]", cell_id, پورټ_id, chunk.دننه_واټن + chunk.پلنوالی - 1, chunk.دننه_واټن);
}

std::string log_signal(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.پلنوالی == 1)
		أرجع stringf("<marker %d> [%d]", chunk.marker, chunk.دننه_واټن);
	أرجع stringf("<marker %d> [%d:%d]", chunk.marker, chunk.دننه_واټن + chunk.پلنوالی - 1, chunk.دننه_واټن);
}

std::string log_signal(DriveChunk ثابت &chunk)
{
	بدل (chunk.type())
	{
		حالة DriveType::NONE:
			أرجع stringf("<none x%d>", chunk.اندازه());
		حالة DriveType::CONSTANT:
			أرجع log_const(chunk.constant());
		حالة DriveType::WIRE:
			أرجع log_signal(chunk.wire());
		حالة DriveType::پورټ_2:
			أرجع log_signal(chunk.پورټ());
		حالة DriveType::MARKER:
			أرجع log_signal(chunk.marker());
		حالة DriveType::مضرب: {
			std::string str = "<multiple";
			ثابت حرف *sep = " ";
			لكل (خپلکاری ثابت &single : chunk.مضرب_2().مضرب_2()) {
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
	خپلکاری &chunks = spec.chunks();
	إذا (chunks.تشه())
		أرجع "{}";
	إذا (chunks.اندازه() == 1)
		أرجع log_signal(chunks[0]);

	std::string str;
	ثابت حرف *sep = "{ ";

	لكل (خپلکاری i = chunks.rbegin(), پای_ټکی = chunks.rend(); i != پای_ټکی; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	أرجع str;
}

YOSYS_NAMESPACE_پای_ټکی
