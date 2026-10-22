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

Drive_بیت::Drive_بیت(Sig_بیت ثابت &بیت)
{
	إذا (بیت.is_wire())
		*هذا = Drive_بیت_Wire(بیت.wire, بیت.جابه_جایی);
	وإلا
		*هذا = بیت.داده;
}

فراغ Drive_بیت::ادغام(Drive_بیت ثابت &other)
{
	إذا (other.type_ == DriveType::NONE)
		أرجع;
	إذا (type_ == DriveType::NONE) {
		*هذا = other;
		أرجع;
	}
	إذا (type_ != DriveType::مضرب) {
		Drive_بیت_مضرب multi(std::move(*هذا));
		*هذا = std::move(multi);
	}
	مضرب_2().ادغام(other);
}


فراغ Drive_بیت_مضرب::ادغام(Drive_بیت ثابت &single)
{
	إذا (single.type() == DriveType::NONE)
		أرجع;
	إذا (single.type() == DriveType::مضرب) {
		ادغام(single.مضرب_2());
		أرجع;
	}
	مضرب_3.emplace(single);
}

فراغ Drive_بیت_مضرب::ادغام(Drive_بیت &&single)
{
	إذا (single.type() == DriveType::NONE)
		أرجع;
	إذا (single.type() == DriveType::مضرب) {
		ادغام(std::move(single.مضرب_2()));
		أرجع;
	}
	مضرب_3.emplace(std::move(single));
}

Drive_بیت_مضرب Drive_Chunk_مضرب::عملگر[](صحيح i) ثابت
{
	Drive_بیت_مضرب نتیجه;
	لكل (خودکار ثابت &single : مضرب_3)
		نتیجه.ادغام(single[i]);
	أرجع نتیجه;
}


منطقي DriveChunkWire::can_append(Drive_بیت_Wire ثابت &بیت) ثابت
{
	أرجع بیت.wire == wire && بیت.جابه_جایی == جابه_جایی + عرض;
}

منطقي DriveChunkWire::try_append(Drive_بیت_Wire ثابت &بیت)
{
	إذا (!can_append(بیت))
		أرجع خطأ;
	عرض += 1;
	أرجع درست;
}

منطقي DriveChunkWire::try_append(DriveChunkWire ثابت &chunk)
{
	إذا (chunk.wire != wire || chunk.جابه_جایی != جابه_جایی + عرض)
		أرجع خطأ;
	عرض += chunk.عرض;
	أرجع درست;
}

منطقي Drive_Chunk_پورت::can_append(Drive_بیت_پورت ثابت &بیت) ثابت
{
	أرجع بیت.cell == cell && بیت.پورت == پورت && بیت.جابه_جایی == جابه_جایی + عرض;
}

منطقي Drive_Chunk_پورت::try_append(Drive_بیت_پورت ثابت &بیت)
{
	إذا (!can_append(بیت))
		أرجع خطأ;
	عرض += 1;
	أرجع درست;
}

منطقي Drive_Chunk_پورت::try_append(Drive_Chunk_پورت ثابت &chunk)
{
	إذا (chunk.cell != cell || chunk.پورت != پورت || chunk.جابه_جایی != جابه_جایی + عرض)
		أرجع خطأ;
	عرض += chunk.عرض;
	أرجع درست;
}

منطقي DriveChunkMarker::can_append(Drive_بیت_Marker ثابت &بیت) ثابت
{
	أرجع بیت.marker == marker && بیت.جابه_جایی == جابه_جایی + عرض;
}

منطقي DriveChunkMarker::try_append(Drive_بیت_Marker ثابت &بیت)
{
	إذا (!can_append(بیت))
		أرجع خطأ;
	عرض += 1;
	أرجع درست;
}

منطقي DriveChunkMarker::try_append(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.marker != marker || chunk.جابه_جایی != جابه_جایی + عرض)
		أرجع خطأ;
	عرض += chunk.عرض;
	أرجع درست;
}


منطقي Drive_Chunk_مضرب::can_append(Drive_بیت_مضرب ثابت &بیت) ثابت
{
	إذا (بیت.مضرب_2().اندازه() != مضرب_3.اندازه())
		أرجع خطأ;

	صحيح const_drivers = 0;
	لكل (DriveChunk ثابت &single : مضرب_3)
		إذا (single.is_constant())
			const_drivers += 1;

	إذا (const_drivers > 1)
		أرجع خطأ;

	لكل (Drive_بیت ثابت &single : بیت.مضرب_2())
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
				Drive_بیت بعدی = Drive_بیت_Wire(wire.wire, wire.جابه_جایی + wire.عرض);
				إذا (!بیت.مضرب_2().شمار(بعدی))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::پورت_2: {
				خودکار ثابت &پورت = single.پورت();
				Drive_بیت بعدی = Drive_بیت_پورت(پورت.cell, پورت.پورت, پورت.جابه_جایی + پورت.عرض);
				إذا (!بیت.مضرب_2().شمار(بعدی))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				خودکار ثابت &marker = single.marker();
				Drive_بیت بعدی = Drive_بیت_Marker(marker.marker, marker.جابه_جایی + marker.عرض);
				إذا (!بیت.مضرب_2().شمار(بعدی))
					أرجع خطأ;
			} اكسر;
			افتراضي:
				أرجع خطأ;
		}
	}

	أرجع درست;
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
				خودکار ثابت &wire = single.wire();
				DriveChunk بعدی = DriveChunkWire(wire.wire, wire.جابه_جایی + wire.عرض, chunk.اندازه());
				إذا (!chunk.مضرب_2().شمار(بعدی))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::پورت_2: {
				خودکار ثابت &پورت = single.پورت();
				DriveChunk بعدی = Drive_Chunk_پورت(پورت.cell, پورت.پورت, پورت.جابه_جایی + پورت.عرض, chunk.اندازه());
				إذا (!chunk.مضرب_2().شمار(بعدی))
					أرجع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				خودکار ثابت &marker = single.marker();
				DriveChunk بعدی = DriveChunkMarker(marker.marker, marker.جابه_جایی + marker.عرض, chunk.اندازه());
				إذا (!chunk.مضرب_2().شمار(بعدی))
					أرجع خطأ;
			} اكسر;
			افتراضي:
				أرجع خطأ;
		}
	}

	أرجع درست;
}

منطقي Drive_Chunk_مضرب::try_append(Drive_بیت_مضرب ثابت &بیت)
{
	إذا (!can_append(بیت))
		أرجع خطأ;
	عرض_3 += 1;
	State constant;

	لكل (Drive_بیت ثابت &single : بیت.مضرب_2())
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
				single.wire().عرض += 1;
			} اكسر;
			حالة DriveType::پورت_2: {
				single.پورت().عرض += 1;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().عرض += 1;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	أرجع درست;
}

منطقي Drive_Chunk_مضرب::try_append(Drive_Chunk_مضرب ثابت &chunk)
{
	إذا (!can_append(chunk))
		أرجع خطأ;
	صحيح عرض = chunk.اندازه();
	عرض_3 += عرض;
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
				single.wire().عرض += عرض;
			} اكسر;
			حالة DriveType::پورت_2: {
				single.پورت().عرض += عرض;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().عرض += عرض;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	أرجع درست;
}

منطقي DriveChunk::can_append(Drive_بیت ثابت &بیت) ثابت
{
	إذا (اندازه() == 0)
		أرجع درست;
	إذا (بیت.type() != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			أرجع درست;
		حالة DriveType::CONSTANT:
			أرجع درست;
		حالة DriveType::WIRE:
			أرجع wire_.can_append(بیت.wire());
		حالة DriveType::پورت_2:
			أرجع پورت_3.can_append(بیت.پورت());
		حالة DriveType::مضرب:
			أرجع مضرب_3.can_append(بیت.مضرب_2());
		افتراضي:
			log_abort();
	}
}

منطقي DriveChunk::try_append(Drive_بیت ثابت &بیت)
{
	إذا (اندازه() == 0)
		*هذا = بیت;
	إذا (بیت.type() != type_)
		أرجع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			none_ += 1;
			أرجع درست;
		حالة DriveType::CONSTANT:
			constant_.append(RTLIL::Const(بیت.constant()));
			أرجع درست;
		حالة DriveType::WIRE:
			أرجع wire_.try_append(بیت.wire());
		حالة DriveType::پورت_2:
			أرجع پورت_3.try_append(بیت.پورت());
		حالة DriveType::مضرب:
			أرجع مضرب_3.try_append(بیت.مضرب_2());
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
			أرجع درست;
		حالة DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			أرجع درست;
		حالة DriveType::WIRE:
			أرجع wire_.try_append(chunk.wire());
		حالة DriveType::پورت_2:
			أرجع پورت_3.try_append(chunk.پورت());
		حالة DriveType::MARKER:
			أرجع marker_.try_append(chunk.marker());
		حالة DriveType::مضرب:
			أرجع مضرب_3.try_append(chunk.مضرب_2());
	}
	log_abort();
}

فراغ DriveSpec::append(Drive_بیت ثابت &بیت)
{
	hash_ = 0;
	إذا (!packed()) {
		بیتها_2.افزودن_پشت(بیت);
		عرض_3 += 1;
		أرجع;
	}

	إذا (chunks_.خالی() || !chunks_.پشت().try_append(بیت))
		chunks_.emplace_پشت(بیت);
	عرض_3 += 1;
}

فراغ DriveSpec::append(DriveChunk ثابت &chunk)
{
	hash_ = 0;
	pack();
	إذا (chunks_.خالی() || !chunks_.پشت().try_append(chunk))
		chunks_.emplace_پشت(chunk);
	عرض_3 += chunk.اندازه();
}

فراغ DriveSpec::pack() ثابت {
	إذا (بیتها_2.خالی())
		أرجع;
	std::وکتور<Drive_بیت> بیتها(std::move(بیتها_2));
	لكل (خودکار &بیت : بیتها)
		إذا (chunks_.خالی() || !chunks_.پشت().try_append(بیت))
			chunks_.emplace_پشت(std::move(بیت));
}

فراغ DriveSpec::unpack() ثابت {
	إذا (chunks_.خالی())
		أرجع;
	لكل (خودکار &chunk : chunks_)
	{
		لكل (صحيح i = 0, عرض = chunk.اندازه(); i != عرض; ++i)
		{
			بیتها_2.emplace_پشت(chunk[i]);
		}
	}
	chunks_.clear();
}

فراغ DriveSpec::compute_عرض()
{
	عرض_3 = 0;
	لكل (خودکار ثابت &chunk : chunks_)
		عرض_3 += chunk.اندازه();
}

فراغ DriverMap::Drive_بیت_گراف::جمع_کن_یال(Drive_بیت_Id src, Drive_بیت_Id dst)
{
	إذا (اولی_یالها.emplace(src, dst).اولی->دومی == dst)
		أرجع;
	إذا (دومی_یالها.emplace(src, dst).اولی->دومی == dst)
		أرجع;
	بیشتر_یالها[src].emplace(dst);
}

DriverMap::Drive_بیت_Id DriverMap::Drive_بیت_گراف::برداشت_یال(Drive_بیت_Id src)
{
	// TODO unused I think?
	خودکار یافت_بیشتر = بیشتر_یالها.پیدا_کن(src);
	إذا (یافت_بیشتر != بیشتر_یالها.انتها()) {
		خودکار نتیجه = یافت_بیشتر->دومی.برداشت();
		إذا (یافت_بیشتر->دومی.خالی())
			بیشتر_یالها.erase(یافت_بیشتر);
		أرجع نتیجه;
	}

	خودکار یافت_دومی = دومی_یالها.پیدا_کن(src);
	إذا (یافت_دومی != دومی_یالها.انتها()) {
		خودکار نتیجه = یافت_دومی->دومی;
		دومی_یالها.erase(یافت_دومی);
		أرجع نتیجه;
	}

	خودکار یافت_اولی = اولی_یالها.پیدا_کن(src);
	إذا (یافت_اولی != اولی_یالها.انتها()) {
		خودکار نتیجه = یافت_اولی->دومی;
		اولی_یالها.erase(یافت_اولی);
		أرجع نتیجه;
	}

	أرجع Drive_بیت_Id();
}

فراغ DriverMap::Drive_بیت_گراف::clear(Drive_بیت_Id src)
{
	اولی_یالها.erase(src);
	دومی_یالها.erase(src);
	بیشتر_یالها.erase(src);
}

منطقي DriverMap::Drive_بیت_گراف::شامل_است(Drive_بیت_Id src)
{
	أرجع اولی_یالها.شمار(src);
}

صحيح DriverMap::Drive_بیت_گراف::شمار(Drive_بیت_Id src)
{
	إذا (!اولی_یالها.شمار(src))
		أرجع 0;
	إذا (!دومی_یالها.شمار(src))
		أرجع 1;
	خودکار یافت = بیشتر_یالها.پیدا_کن(src);
	إذا (یافت == بیشتر_یالها.انتها())
		أرجع 2;
	أرجع Get_اندازه(یافت->دومی) + 2;
}

DriverMap::Drive_بیت_Id DriverMap::Drive_بیت_گراف::at(Drive_بیت_Id src, صحيح شاخص)
{
	إذا (شاخص == 0)
		أرجع اولی_یالها.at(src);
	وإلا إذا (شاخص == 1)
		أرجع دومی_یالها.at(src);
	وإلا
		أرجع *بیشتر_یالها.at(src).element(شاخص - 2);
}


DriverMap::بیت_نما DriverMap::بیت_نما_2(Drive_بیت ثابت &بیت)
{
	بدل (بیت.type())
	{
		حالة DriveType::NONE:
			أرجع بیت_نما::NONE;
		حالة DriveType::CONSTANT:
			// TODO how to handle Sx here?
			أرجع بیت.constant() == State::Sz ? بیت_نما::NONE : بیت_نما::DRIVER;
		حالة DriveType::WIRE: {
			خودکار ثابت &wire = بیت.wire();
			منطقي driver = wire.wire->پورت_درون_داد;
			منطقي driven = wire.wire->پورت_output;

			إذا (driver && !driven)
				أرجع بیت_نما::DRIVER;
			وإلا إذا (driven && !driver)
				أرجع بیت_نما::DRIVEN;
			وإلا إذا (driver && driven)
				أرجع بیت_نما::TRISTATE;
			وإلا
				أرجع keep_wire(بیت.wire().wire) ? بیت_نما::KEEP : بیت_نما::NONE;
		}
		حالة DriveType::پورت_2: {
			خودکار ثابت &پورت = بیت.پورت();
			منطقي driver = celltypes.cell_output(پورت.cell->type, پورت.پورت);
			منطقي driven = celltypes.cell_درون_داد(پورت.cell->type, پورت.پورت);
			إذا (driver && !driven)
				أرجع بیت_نما::DRIVER;
			وإلا إذا (driven && !driver)
				أرجع بیت_نما::DRIVEN_UNIQUE;
			وإلا
				أرجع بیت_نما::TRISTATE;
		}
		حالة DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		افتراضي:
			log_abort();
	}
}

DriverMap::Drive_بیت_Id DriverMap::id_from_drive_بیت(Drive_بیت ثابت &بیت)
{
	بدل (بیت.type())
	{
		حالة DriveType::NONE:
			أرجع -1;
		حالة DriveType::CONSTANT:
			أرجع (صحيح)بیت.constant();
		حالة DriveType::WIRE: {
			خودکار ثابت &wire_بیت = بیت.wire();
			صحيح جابه_جایی = بعدی_جابه_جایی;
			خودکار insertion = wire_offsets.emplace(wire_بیت.wire, جابه_جایی);
			إذا (insertion.دومی) {
				إذا (wire_بیت.wire->عرض == 1) {
					log_assert(wire_بیت.جابه_جایی == 0);
					isolated_drive_بیتها.emplace(جابه_جایی, بیت);
				} وإلا
					drive_بیتها.emplace(جابه_جایی, Drive_بیت_Wire(wire_بیت.wire, 0));
				بعدی_جابه_جایی += wire_بیت.wire->عرض;
			}
			أرجع insertion.اولی->دومی.id + wire_بیت.جابه_جایی;
		}
		حالة DriveType::پورت_2: {
			خودکار ثابت &پورت_بیت = بیت.پورت();
			خودکار کلید = std::make_pair(پورت_بیت.cell, پورت_بیت.پورت);
			صحيح جابه_جایی = بعدی_جابه_جایی;
			خودکار insertion = پورت_offsets.emplace(کلید, جابه_جایی);
			إذا (insertion.دومی) {
				صحيح عرض = پورت_بیت.cell->connections().at(پورت_بیت.پورت).اندازه();
				إذا (عرض == 1 && جابه_جایی == 0) {
					log_assert(پورت_بیت.جابه_جایی == 0);
					isolated_drive_بیتها.emplace(جابه_جایی, بیت);
				} وإلا
					drive_بیتها.emplace(جابه_جایی, Drive_بیت_پورت(پورت_بیت.cell, پورت_بیت.پورت, 0));
				بعدی_جابه_جایی += عرض;
			}
			أرجع insertion.اولی->دومی.id + پورت_بیت.جابه_جایی;
		}
		افتراضي:
			log_assert(خطأ && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

Drive_بیت DriverMap::drive_بیت_from_id(Drive_بیت_Id id)
{
	خودکار یافت_isolated = isolated_drive_بیتها.پیدا_کن(id);
	إذا (یافت_isolated != isolated_drive_بیتها.انتها())
		أرجع یافت_isolated->دومی;

	خودکار یافت = drive_بیتها.فوقانی_bound(id);
	إذا (یافت == drive_بیتها.begin()) {
		أرجع id < 0 ? Drive_بیت() : Drive_بیت((State) id.id);
	}
	--یافت;
	Drive_بیت نتیجه = یافت->دومی;
	إذا (نتیجه.is_wire()) {
		نتیجه.wire().جابه_جایی += id.id - یافت->اولی.id;
	} وإلا {
		log_assert(نتیجه.is_پورت());
		نتیجه.پورت().جابه_جایی += id.id - یافت->اولی.id;
	}
	أرجع نتیجه;
}

فراغ DriverMap::connect_directed_ادغام(Drive_بیت_Id driven_id, Drive_بیت_Id driver_id)
{
	إذا (driven_id == driver_id)
		أرجع;

	same_driver.ادغام(driven_id, driver_id);

	لكل (صحيح i = 0, انتها = connected_drivers.شمار(driven_id); i != انتها; ++i)
		connected_drivers.جمع_کن_یال(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	لكل (صحيح i = 0, انتها = connected_undirected.شمار(driven_id); i != انتها; ++i)
		connected_undirected.جمع_کن_یال(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

فراغ DriverMap::connect_directed_میانگیر(Drive_بیت_Id driven_id, Drive_بیت_Id driver_id)
{
	connected_drivers.جمع_کن_یال(driven_id, driver_id);
}

فراغ DriverMap::connect_undirected(Drive_بیت_Id a_id, Drive_بیت_Id b_id)
{
	connected_undirected.جمع_کن_یال(a_id, b_id);
	connected_undirected.جمع_کن_یال(b_id, a_id);
}

فراغ DriverMap::جمع_کن(Module *module)
{
	لكل (خودکار ثابت &conn : module->connections())
		جمع_کن(conn.اولی, conn.دومی);

	لكل (خودکار cell : module->cells())
		لكل (خودکار ثابت &conn : cell->connections())
			جمع_کن_پورت(cell, conn.اولی, conn.دومی);
}

// Add a single bit connection to the driver map.
فراغ DriverMap::جمع_کن(Drive_بیت ثابت &a, Drive_بیت ثابت &b)
{
	Drive_بیت_Id a_id = id_from_drive_بیت(a);
	Drive_بیت_Id b_id = id_from_drive_بیت(b);

	Drive_بیت_Id orig_a_id = a_id;
	Drive_بیت_Id orig_b_id = b_id;

	a_id = same_driver.پیدا_کن(a_id);
	b_id = same_driver.پیدا_کن(b_id);

	إذا (a_id == b_id)
		أرجع;

	بیت_نما a_نما = بیت_نما_2(orig_a_id == a_id ? a : drive_بیت_from_id(a_id));
	بیت_نما b_نما = بیت_نما_2(orig_b_id == b_id ? b : drive_بیت_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	إذا (a_نما == بیت_نما::NONE && !(b_نما == بیت_نما::DRIVEN_UNIQUE || b_نما == بیت_نما::DRIVEN))
		connect_directed_ادغام(a_id, b_id);
	وإلا إذا (b_نما == بیت_نما::NONE && !(a_نما == بیت_نما::DRIVEN_UNIQUE || a_نما == بیت_نما::DRIVEN))
		connect_directed_ادغام(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	وإلا إذا (a_نما == بیت_نما::DRIVEN_UNIQUE && !(b_نما == بیت_نما::DRIVEN_UNIQUE || b_نما == بیت_نما::DRIVEN))
		connect_directed_میانگیر(a_id, b_id);
	وإلا إذا (b_نما == بیت_نما::DRIVEN_UNIQUE && !(a_نما == بیت_نما::DRIVEN_UNIQUE || a_نما == بیت_نما::DRIVEN))
		connect_directed_میانگیر(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	وإلا إذا (a_نما == بیت_نما::DRIVER)
		connect_directed_میانگیر(b_id, a_id);
	وإلا إذا (b_نما == بیت_نما::DRIVER)
		connect_directed_میانگیر(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	وإلا
		connect_undirected(a_id, b_id);

	أرجع;
}

// Specialized version that avoids unpacking
فراغ DriverMap::جمع_کن(SigSpec ثابت &a, SigSpec ثابت &b)
{
	log_assert(a.اندازه() == b.اندازه());
	خودکار ثابت &a_chunks = a.chunks();
	خودکار ثابت &b_chunks = b.chunks();

	خودکار a_chunk = a_chunks.begin();
	خودکار a_انتها = a_chunks.انتها();
	صحيح a_جابه_جایی = 0;

	خودکار b_chunk = b_chunks.begin();
	صحيح b_جابه_جایی = 0;

	SigChunk tmp_a, tmp_b;
	طالما (a_chunk != a_انتها) {
		صحيح a_عرض = a_chunk->عرض - a_جابه_جایی;
		إذا (a_عرض == 0) {
			a_جابه_جایی = 0;
			++a_chunk;
			استمر;
		}
		صحيح b_عرض = b_chunk->عرض - b_جابه_جایی;
		إذا (b_عرض == 0) {
			b_جابه_جایی = 0;
			++b_chunk;
			استمر;
		}
		صحيح عرض = std::min(a_عرض, b_عرض);
		log_assert(عرض > 0);

		SigChunk ثابت &a_subchunk =
			a_جابه_جایی == 0 && a_عرض == عرض ? *a_chunk : a_chunk->استخراج(a_جابه_جایی, عرض);
		SigChunk ثابت &b_subchunk =
			b_جابه_جایی == 0 && b_عرض == عرض ? *b_chunk : b_chunk->استخراج(b_جابه_جایی, عرض);

		جمع_کن(a_subchunk, b_subchunk);

		a_جابه_جایی += عرض;
		b_جابه_جایی += عرض;
	}
}

فراغ DriverMap::جمع_کن_پورت(Cell *cell, IdString پورت, SigSpec ثابت &b)
{
	صحيح جابه_جایی = 0;
	لكل (خودکار ثابت &chunk : b.chunks()) {
		جمع_کن(chunk, Drive_Chunk_پورت(cell, پورت, جابه_جایی, chunk.عرض));
		جابه_جایی += chunk.اندازه();
	}
}

فراغ DriverMap::orient_undirected(Drive_بیت_Id id)
{
	pool<Drive_بیت_Id> &دیده = orient_undirected_دیده;
	pool<Drive_بیت_Id> &drivers = orient_undirected_drivers;
	dict<Drive_بیت_Id, صحيح> &فاصله = orient_undirected_فاصله;
	دیده.clear();
	drivers.clear();

	دیده.emplace(id);

	لكل (صحيح pos = 0; pos < Get_اندازه(دیده); ++pos) {
		Drive_بیت_Id current = *دیده.element(دیده.اندازه() - 1 - pos);
		Drive_بیت بیت = drive_بیت_from_id(current);

		بیت_نما نما = بیت_نما_2(بیت);

		إذا (نما == بیت_نما::DRIVER || نما == بیت_نما::TRISTATE)
			drivers.emplace(current);

		إذا (connected_drivers.شامل_است(current))
			drivers.emplace(current);

		صحيح undirected_driver_شمار = connected_undirected.شمار(current);

		لكل (صحيح i = 0; i != undirected_driver_شمار; ++i)
			دیده.emplace(same_driver.پیدا_کن(connected_undirected.at(current, i)));
	}

	إذا (drivers.خالی())
		لكل (خودکار دیده_id : دیده)
			drivers.emplace(دیده_id);

	لكل (خودکار driver : drivers)
	{
		فاصله.clear();
		فاصله.emplace(driver, 0);

		لكل (صحيح pos = 0; pos < Get_اندازه(فاصله); ++pos) {
			خودکار current_it = فاصله.element(فاصله.اندازه() - 1 - pos);

			Drive_بیت_Id current = current_it->اولی;
			صحيح undirected_driver_شمار = connected_undirected.شمار(current);

			لكل (صحيح i = 0; i != undirected_driver_شمار; ++i)
			{
				Drive_بیت_Id بعدی = same_driver.پیدا_کن(connected_undirected.at(current, i));
				خودکار emplaced = فاصله.emplace(بعدی, current_it->دومی + 1);
				إذا (emplaced.اولی->دومی == current_it->دومی + 1)
					connected_oriented.جمع_کن_یال(بعدی, current);
			}
		}
	}

	لكل (خودکار دیده_id : دیده)
		oriented_present.emplace(دیده_id);
}

Drive_بیت DriverMap::عملگر()(Drive_بیت ثابت &بیت)
{
	إذا (بیت.type() == DriveType::MARKER || بیت.type() == DriveType::NONE)
		أرجع بیت;
	إذا (بیت.type() == DriveType::مضرب)
	{
		Drive_بیت نتیجه;
		لكل (خودکار ثابت &inner : بیت.مضرب_2().مضرب_2())
			نتیجه.ادغام((*هذا)(inner));
		أرجع نتیجه;
	}

	Drive_بیت_Id بیت_id = id_from_drive_بیت(بیت);

	Drive_بیت_Id بیت_repr_id = same_driver.پیدا_کن(بیت_id);

	Drive_بیت بیت_repr = drive_بیت_from_id(بیت_repr_id);

	بیت_نما نما = بیت_نما_2(بیت_repr);

	إذا (نما == بیت_نما::KEEP && بیت_repr_id != بیت_id)
		أرجع بیت_repr;

	صحيح implicit_driver_شمار = connected_drivers.شمار(بیت_repr_id);
	إذا (connected_undirected.شامل_است(بیت_repr_id) && !oriented_present.شمار(بیت_repr_id))
		orient_undirected(بیت_repr_id);

	Drive_بیت driver;

	إذا (نما == بیت_نما::DRIVER || نما == بیت_نما::TRISTATE)
		driver = بیت_repr;

	لكل (صحيح i = 0; i != implicit_driver_شمار; ++i)
		driver.ادغام(drive_بیت_from_id(connected_drivers.at(بیت_repr_id, i)));

	صحيح oriented_driver_شمار = connected_oriented.شمار(بیت_repr_id);
	لكل (صحيح i = 0; i != oriented_driver_شمار; ++i)
		driver.ادغام(drive_بیت_from_id(connected_oriented.at(بیت_repr_id, i)));

	أرجع driver;
}

DriveSpec DriverMap::عملگر()(DriveSpec spec)
{
	DriveSpec نتیجه;

	لكل (صحيح i = 0, عرض = spec.اندازه(); i != عرض; ++i)
		نتیجه.append((*هذا)(spec[i]));

	أرجع نتیجه;
}

std::string log_signal(DriveChunkWire ثابت &chunk)
{
	std::string id = chunk.wire->name.unescape();
	إذا (chunk.is_کامل())
		أرجع id;
	إذا (chunk.عرض == 1)
		أرجع stringf("%s [%d]", id, chunk.جابه_جایی);
	أرجع stringf("%s [%d:%d]", id, chunk.جابه_جایی + chunk.عرض - 1, chunk.جابه_جایی);
}


std::string log_signal(Drive_Chunk_پورت ثابت &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string پورت_id = chunk.cell->twines().unescaped_str(chunk.پورت);
	إذا (chunk.is_کامل())
		أرجع stringf("%s <%s>", cell_id, پورت_id);
	إذا (chunk.عرض == 1)
		أرجع stringf("%s <%s> [%d]", cell_id, پورت_id, chunk.جابه_جایی);
	أرجع stringf("%s <%s> [%d:%d]", cell_id, پورت_id, chunk.جابه_جایی + chunk.عرض - 1, chunk.جابه_جایی);
}

std::string log_signal(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.عرض == 1)
		أرجع stringf("<marker %d> [%d]", chunk.marker, chunk.جابه_جایی);
	أرجع stringf("<marker %d> [%d:%d]", chunk.marker, chunk.جابه_جایی + chunk.عرض - 1, chunk.جابه_جایی);
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
		حالة DriveType::پورت_2:
			أرجع log_signal(chunk.پورت());
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
	إذا (chunks.خالی())
		أرجع "{}";
	إذا (chunks.اندازه() == 1)
		أرجع log_signal(chunks[0]);

	std::string str;
	ثابت حرف *sep = "{ ";

	لكل (خودکار i = chunks.rbegin(), انتها = chunks.rend(); i != انتها; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	أرجع str;
}

YOSYS_NAMESPACE_انتها
