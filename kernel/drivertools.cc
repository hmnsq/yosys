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

#شامل "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

Drive_ثنائی_ہندسہ::Drive_ثنائی_ہندسہ(Sig_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	اگر (ثنائی_ہندسہ.is_wire())
		*یہ = Drive_ثنائی_ہندسہ_Wire(ثنائی_ہندسہ.wire, ثنائی_ہندسہ.انحراف);
	ورنہ
		*یہ = ثنائی_ہندسہ.ڈیٹا;
}

خالی Drive_ثنائی_ہندسہ::ملاؤ(Drive_ثنائی_ہندسہ ثابت &other)
{
	اگر (other.type_ == DriveType::NONE)
		واپس;
	اگر (type_ == DriveType::NONE) {
		*یہ = other;
		واپس;
	}
	اگر (type_ != DriveType::مضرب) {
		Drive_ثنائی_ہندسہ_مضرب multi(std::move(*یہ));
		*یہ = std::move(multi);
	}
	مضرب_2().ملاؤ(other);
}


خالی Drive_ثنائی_ہندسہ_مضرب::ملاؤ(Drive_ثنائی_ہندسہ ثابت &single)
{
	اگر (single.type() == DriveType::NONE)
		واپس;
	اگر (single.type() == DriveType::مضرب) {
		ملاؤ(single.مضرب_2());
		واپس;
	}
	مضرب_3.emplace(single);
}

خالی Drive_ثنائی_ہندسہ_مضرب::ملاؤ(Drive_ثنائی_ہندسہ &&single)
{
	اگر (single.type() == DriveType::NONE)
		واپس;
	اگر (single.type() == DriveType::مضرب) {
		ملاؤ(std::move(single.مضرب_2()));
		واپس;
	}
	مضرب_3.emplace(std::move(single));
}

Drive_ثنائی_ہندسہ_مضرب Drive_Chunk_مضرب::عامل[](صحیح i) ثابت
{
	Drive_ثنائی_ہندسہ_مضرب نتیجہ;
	برائے (خودکار ثابت &single : مضرب_3)
		نتیجہ.ملاؤ(single[i]);
	واپس نتیجہ;
}


منطقی DriveChunkWire::can_append(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ) ثابت
{
	واپس ثنائی_ہندسہ.wire == wire && ثنائی_ہندسہ.انحراف == انحراف + چوڑائی;
}

منطقی DriveChunkWire::try_append(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ)
{
	اگر (!can_append(ثنائی_ہندسہ))
		واپس جھوٹ;
	چوڑائی += 1;
	واپس سچ;
}

منطقی DriveChunkWire::try_append(DriveChunkWire ثابت &chunk)
{
	اگر (chunk.wire != wire || chunk.انحراف != انحراف + چوڑائی)
		واپس جھوٹ;
	چوڑائی += chunk.چوڑائی;
	واپس سچ;
}

منطقی Drive_Chunk_پورٹ::can_append(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ) ثابت
{
	واپس ثنائی_ہندسہ.cell == cell && ثنائی_ہندسہ.پورٹ == پورٹ && ثنائی_ہندسہ.انحراف == انحراف + چوڑائی;
}

منطقی Drive_Chunk_پورٹ::try_append(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ)
{
	اگر (!can_append(ثنائی_ہندسہ))
		واپس جھوٹ;
	چوڑائی += 1;
	واپس سچ;
}

منطقی Drive_Chunk_پورٹ::try_append(Drive_Chunk_پورٹ ثابت &chunk)
{
	اگر (chunk.cell != cell || chunk.پورٹ != پورٹ || chunk.انحراف != انحراف + چوڑائی)
		واپس جھوٹ;
	چوڑائی += chunk.چوڑائی;
	واپس سچ;
}

منطقی DriveChunkMarker::can_append(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ) ثابت
{
	واپس ثنائی_ہندسہ.marker == marker && ثنائی_ہندسہ.انحراف == انحراف + چوڑائی;
}

منطقی DriveChunkMarker::try_append(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ)
{
	اگر (!can_append(ثنائی_ہندسہ))
		واپس جھوٹ;
	چوڑائی += 1;
	واپس سچ;
}

منطقی DriveChunkMarker::try_append(DriveChunkMarker ثابت &chunk)
{
	اگر (chunk.marker != marker || chunk.انحراف != انحراف + چوڑائی)
		واپس جھوٹ;
	چوڑائی += chunk.چوڑائی;
	واپس سچ;
}


منطقی Drive_Chunk_مضرب::can_append(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ) ثابت
{
	اگر (ثنائی_ہندسہ.مضرب_2().سائز() != مضرب_3.سائز())
		واپس جھوٹ;

	صحیح const_drivers = 0;
	برائے (DriveChunk ثابت &single : مضرب_3)
		اگر (single.is_constant())
			const_drivers += 1;

	اگر (const_drivers > 1)
		واپس جھوٹ;

	برائے (Drive_ثنائی_ہندسہ ثابت &single : ثنائی_ہندسہ.مضرب_2())
		اگر (single.is_constant())
			const_drivers -= 1;

	اگر (const_drivers != 0)
		واپس جھوٹ;

	برائے (DriveChunk ثابت &single : مضرب_3)
	{
		انتخاب (single.type())
		{
			حالت DriveType::CONSTANT: {
			} توڑو;
			حالت DriveType::WIRE: {
				خودکار ثابت &wire = single.wire();
				Drive_ثنائی_ہندسہ اگلا = Drive_ثنائی_ہندسہ_Wire(wire.wire, wire.انحراف + wire.چوڑائی);
				اگر (!ثنائی_ہندسہ.مضرب_2().گنتی(اگلا))
					واپس جھوٹ;
			} توڑو;
			حالت DriveType::پورٹ_2: {
				خودکار ثابت &پورٹ = single.پورٹ();
				Drive_ثنائی_ہندسہ اگلا = Drive_ثنائی_ہندسہ_پورٹ(پورٹ.cell, پورٹ.پورٹ, پورٹ.انحراف + پورٹ.چوڑائی);
				اگر (!ثنائی_ہندسہ.مضرب_2().گنتی(اگلا))
					واپس جھوٹ;
			} توڑو;
			حالت DriveType::MARKER: {
				خودکار ثابت &marker = single.marker();
				Drive_ثنائی_ہندسہ اگلا = Drive_ثنائی_ہندسہ_Marker(marker.marker, marker.انحراف + marker.چوڑائی);
				اگر (!ثنائی_ہندسہ.مضرب_2().گنتی(اگلا))
					واپس جھوٹ;
			} توڑو;
			طےشدہ:
				واپس جھوٹ;
		}
	}

	واپس سچ;
}

منطقی Drive_Chunk_مضرب::can_append(Drive_Chunk_مضرب ثابت &chunk) ثابت
{
	اگر (chunk.مضرب_2().سائز() != مضرب_3.سائز())
		واپس جھوٹ;

	صحیح const_drivers = 0;
	برائے (DriveChunk ثابت &single : مضرب_3)
		اگر (single.is_constant())
			const_drivers += 1;

	اگر (const_drivers > 1)
		واپس جھوٹ;

	برائے (DriveChunk ثابت &single : chunk.مضرب_2())
		اگر (single.is_constant())
			const_drivers -= 1;

	اگر (const_drivers != 0)
		واپس جھوٹ;

	برائے (DriveChunk ثابت &single : مضرب_3)
	{
		انتخاب (single.type())
		{
			حالت DriveType::CONSTANT: {
			} توڑو;
			حالت DriveType::WIRE: {
				خودکار ثابت &wire = single.wire();
				DriveChunk اگلا = DriveChunkWire(wire.wire, wire.انحراف + wire.چوڑائی, chunk.سائز());
				اگر (!chunk.مضرب_2().گنتی(اگلا))
					واپس جھوٹ;
			} توڑو;
			حالت DriveType::پورٹ_2: {
				خودکار ثابت &پورٹ = single.پورٹ();
				DriveChunk اگلا = Drive_Chunk_پورٹ(پورٹ.cell, پورٹ.پورٹ, پورٹ.انحراف + پورٹ.چوڑائی, chunk.سائز());
				اگر (!chunk.مضرب_2().گنتی(اگلا))
					واپس جھوٹ;
			} توڑو;
			حالت DriveType::MARKER: {
				خودکار ثابت &marker = single.marker();
				DriveChunk اگلا = DriveChunkMarker(marker.marker, marker.انحراف + marker.چوڑائی, chunk.سائز());
				اگر (!chunk.مضرب_2().گنتی(اگلا))
					واپس جھوٹ;
			} توڑو;
			طےشدہ:
				واپس جھوٹ;
		}
	}

	واپس سچ;
}

منطقی Drive_Chunk_مضرب::try_append(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ)
{
	اگر (!can_append(ثنائی_ہندسہ))
		واپس جھوٹ;
	چوڑائی_3 += 1;
	State constant;

	برائے (Drive_ثنائی_ہندسہ ثابت &single : ثنائی_ہندسہ.مضرب_2())
		اگر (single.is_constant())
			constant = single.constant();

	برائے (DriveChunk &single : مضرب_3)
	{
		انتخاب (single.type())
		{
			حالت DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} توڑو;
			حالت DriveType::WIRE: {
				single.wire().چوڑائی += 1;
			} توڑو;
			حالت DriveType::پورٹ_2: {
				single.پورٹ().چوڑائی += 1;
			} توڑو;
			حالت DriveType::MARKER: {
				single.marker().چوڑائی += 1;
			} توڑو;
			طےشدہ:
				log_abort();
		}
	}
	واپس سچ;
}

منطقی Drive_Chunk_مضرب::try_append(Drive_Chunk_مضرب ثابت &chunk)
{
	اگر (!can_append(chunk))
		واپس جھوٹ;
	صحیح چوڑائی = chunk.سائز();
	چوڑائی_3 += چوڑائی;
	Const constant;

	برائے (DriveChunk ثابت &single : chunk.مضرب_2())
		اگر (single.is_constant())
			constant = single.constant();

	برائے (DriveChunk &single : مضرب_3)
	{
		انتخاب (single.type())
		{
			حالت DriveType::CONSTANT: {
				single.constant().append(constant);
			} توڑو;
			حالت DriveType::WIRE: {
				single.wire().چوڑائی += چوڑائی;
			} توڑو;
			حالت DriveType::پورٹ_2: {
				single.پورٹ().چوڑائی += چوڑائی;
			} توڑو;
			حالت DriveType::MARKER: {
				single.marker().چوڑائی += چوڑائی;
			} توڑو;
			طےشدہ:
				log_abort();
		}
	}
	واپس سچ;
}

منطقی DriveChunk::can_append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ) ثابت
{
	اگر (سائز() == 0)
		واپس سچ;
	اگر (ثنائی_ہندسہ.type() != type_)
		واپس جھوٹ;
	انتخاب (type_)
	{
		حالت DriveType::NONE:
			واپس سچ;
		حالت DriveType::CONSTANT:
			واپس سچ;
		حالت DriveType::WIRE:
			واپس wire_.can_append(ثنائی_ہندسہ.wire());
		حالت DriveType::پورٹ_2:
			واپس پورٹ_3.can_append(ثنائی_ہندسہ.پورٹ());
		حالت DriveType::مضرب:
			واپس مضرب_3.can_append(ثنائی_ہندسہ.مضرب_2());
		طےشدہ:
			log_abort();
	}
}

منطقی DriveChunk::try_append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	اگر (سائز() == 0)
		*یہ = ثنائی_ہندسہ;
	اگر (ثنائی_ہندسہ.type() != type_)
		واپس جھوٹ;
	انتخاب (type_)
	{
		حالت DriveType::NONE:
			none_ += 1;
			واپس سچ;
		حالت DriveType::CONSTANT:
			constant_.append(RTLIL::Const(ثنائی_ہندسہ.constant()));
			واپس سچ;
		حالت DriveType::WIRE:
			واپس wire_.try_append(ثنائی_ہندسہ.wire());
		حالت DriveType::پورٹ_2:
			واپس پورٹ_3.try_append(ثنائی_ہندسہ.پورٹ());
		حالت DriveType::مضرب:
			واپس مضرب_3.try_append(ثنائی_ہندسہ.مضرب_2());
		طےشدہ:
			log_abort();
	}
}


منطقی DriveChunk::try_append(DriveChunk ثابت &chunk)
{
	اگر (سائز() == 0)
		*یہ = chunk;
	اگر (chunk.type_ != type_)
		واپس جھوٹ;
	انتخاب (type_)
	{
		حالت DriveType::NONE:
			none_ += chunk.none_;
			واپس سچ;
		حالت DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			واپس سچ;
		حالت DriveType::WIRE:
			واپس wire_.try_append(chunk.wire());
		حالت DriveType::پورٹ_2:
			واپس پورٹ_3.try_append(chunk.پورٹ());
		حالت DriveType::MARKER:
			واپس marker_.try_append(chunk.marker());
		حالت DriveType::مضرب:
			واپس مضرب_3.try_append(chunk.مضرب_2());
	}
	log_abort();
}

خالی DriveSpec::append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	hash_ = 0;
	اگر (!packed()) {
		بٹ_2.ڈالو_عقب(ثنائی_ہندسہ);
		چوڑائی_3 += 1;
		واپس;
	}

	اگر (chunks_.خالی_خانے() || !chunks_.عقب().try_append(ثنائی_ہندسہ))
		chunks_.emplace_عقب(ثنائی_ہندسہ);
	چوڑائی_3 += 1;
}

خالی DriveSpec::append(DriveChunk ثابت &chunk)
{
	hash_ = 0;
	pack();
	اگر (chunks_.خالی_خانے() || !chunks_.عقب().try_append(chunk))
		chunks_.emplace_عقب(chunk);
	چوڑائی_3 += chunk.سائز();
}

خالی DriveSpec::pack() ثابت {
	اگر (بٹ_2.خالی_خانے())
		واپس;
	std::سمتیہ<Drive_ثنائی_ہندسہ> بٹ(std::move(بٹ_2));
	برائے (خودکار &ثنائی_ہندسہ : بٹ)
		اگر (chunks_.خالی_خانے() || !chunks_.عقب().try_append(ثنائی_ہندسہ))
			chunks_.emplace_عقب(std::move(ثنائی_ہندسہ));
}

خالی DriveSpec::unpack() ثابت {
	اگر (chunks_.خالی_خانے())
		واپس;
	برائے (خودکار &chunk : chunks_)
	{
		برائے (صحیح i = 0, چوڑائی = chunk.سائز(); i != چوڑائی; ++i)
		{
			بٹ_2.emplace_عقب(chunk[i]);
		}
	}
	chunks_.clear();
}

خالی DriveSpec::compute_چوڑائی()
{
	چوڑائی_3 = 0;
	برائے (خودکار ثابت &chunk : chunks_)
		چوڑائی_3 += chunk.سائز();
}

خالی DriverMap::Drive_ثنائی_ہندسہ_گراف::جوڑو_کنارہ(Drive_ثنائی_ہندسہ_Id src, Drive_ثنائی_ہندسہ_Id dst)
{
	اگر (پہلا_کنارے.emplace(src, dst).پہلا->دوسرا == dst)
		واپس;
	اگر (دوسرا_کنارے.emplace(src, dst).پہلا->دوسرا == dst)
		واپس;
	مزید_کنارے[src].emplace(dst);
}

DriverMap::Drive_ثنائی_ہندسہ_Id DriverMap::Drive_ثنائی_ہندسہ_گراف::نکالو_کنارہ(Drive_ثنائی_ہندسہ_Id src)
{
	// TODO unused I think?
	خودکار ملا_مزید = مزید_کنارے.ڈھونڈو(src);
	اگر (ملا_مزید != مزید_کنارے.انتہا()) {
		خودکار نتیجہ = ملا_مزید->دوسرا.نکالو();
		اگر (ملا_مزید->دوسرا.خالی_خانے())
			مزید_کنارے.erase(ملا_مزید);
		واپس نتیجہ;
	}

	خودکار ملا_دوسرا = دوسرا_کنارے.ڈھونڈو(src);
	اگر (ملا_دوسرا != دوسرا_کنارے.انتہا()) {
		خودکار نتیجہ = ملا_دوسرا->دوسرا;
		دوسرا_کنارے.erase(ملا_دوسرا);
		واپس نتیجہ;
	}

	خودکار ملا_پہلا = پہلا_کنارے.ڈھونڈو(src);
	اگر (ملا_پہلا != پہلا_کنارے.انتہا()) {
		خودکار نتیجہ = ملا_پہلا->دوسرا;
		پہلا_کنارے.erase(ملا_پہلا);
		واپس نتیجہ;
	}

	واپس Drive_ثنائی_ہندسہ_Id();
}

خالی DriverMap::Drive_ثنائی_ہندسہ_گراف::clear(Drive_ثنائی_ہندسہ_Id src)
{
	پہلا_کنارے.erase(src);
	دوسرا_کنارے.erase(src);
	مزید_کنارے.erase(src);
}

منطقی DriverMap::Drive_ثنائی_ہندسہ_گراف::شامل_ہے(Drive_ثنائی_ہندسہ_Id src)
{
	واپس پہلا_کنارے.گنتی(src);
}

صحیح DriverMap::Drive_ثنائی_ہندسہ_گراف::گنتی(Drive_ثنائی_ہندسہ_Id src)
{
	اگر (!پہلا_کنارے.گنتی(src))
		واپس 0;
	اگر (!دوسرا_کنارے.گنتی(src))
		واپس 1;
	خودکار ملا = مزید_کنارے.ڈھونڈو(src);
	اگر (ملا == مزید_کنارے.انتہا())
		واپس 2;
	واپس Get_سائز(ملا->دوسرا) + 2;
}

DriverMap::Drive_ثنائی_ہندسہ_Id DriverMap::Drive_ثنائی_ہندسہ_گراف::at(Drive_ثنائی_ہندسہ_Id src, صحیح اشاریہ)
{
	اگر (اشاریہ == 0)
		واپس پہلا_کنارے.at(src);
	ورنہ اگر (اشاریہ == 1)
		واپس دوسرا_کنارے.at(src);
	ورنہ
		واپس *مزید_کنارے.at(src).element(اشاریہ - 2);
}


DriverMap::ثنائی_ہندسہ_منوال DriverMap::ثنائی_ہندسہ_منوال_2(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	انتخاب (ثنائی_ہندسہ.type())
	{
		حالت DriveType::NONE:
			واپس ثنائی_ہندسہ_منوال::NONE;
		حالت DriveType::CONSTANT:
			// TODO how to handle Sx here?
			واپس ثنائی_ہندسہ.constant() == State::Sz ? ثنائی_ہندسہ_منوال::NONE : ثنائی_ہندسہ_منوال::DRIVER;
		حالت DriveType::WIRE: {
			خودکار ثابت &wire = ثنائی_ہندسہ.wire();
			منطقی driver = wire.wire->پورٹ_داخلہ;
			منطقی driven = wire.wire->پورٹ_output;

			اگر (driver && !driven)
				واپس ثنائی_ہندسہ_منوال::DRIVER;
			ورنہ اگر (driven && !driver)
				واپس ثنائی_ہندسہ_منوال::DRIVEN;
			ورنہ اگر (driver && driven)
				واپس ثنائی_ہندسہ_منوال::TRISTATE;
			ورنہ
				واپس keep_wire(ثنائی_ہندسہ.wire().wire) ? ثنائی_ہندسہ_منوال::KEEP : ثنائی_ہندسہ_منوال::NONE;
		}
		حالت DriveType::پورٹ_2: {
			خودکار ثابت &پورٹ = ثنائی_ہندسہ.پورٹ();
			منطقی driver = celltypes.cell_output(پورٹ.cell->type, پورٹ.پورٹ);
			منطقی driven = celltypes.cell_داخلہ(پورٹ.cell->type, پورٹ.پورٹ);
			اگر (driver && !driven)
				واپس ثنائی_ہندسہ_منوال::DRIVER;
			ورنہ اگر (driven && !driver)
				واپس ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE;
			ورنہ
				واپس ثنائی_ہندسہ_منوال::TRISTATE;
		}
		حالت DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		طےشدہ:
			log_abort();
	}
}

DriverMap::Drive_ثنائی_ہندسہ_Id DriverMap::id_from_drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	انتخاب (ثنائی_ہندسہ.type())
	{
		حالت DriveType::NONE:
			واپس -1;
		حالت DriveType::CONSTANT:
			واپس (صحیح)ثنائی_ہندسہ.constant();
		حالت DriveType::WIRE: {
			خودکار ثابت &wire_ثنائی_ہندسہ = ثنائی_ہندسہ.wire();
			صحیح انحراف = اگلا_انحراف;
			خودکار insertion = wire_offsets.emplace(wire_ثنائی_ہندسہ.wire, انحراف);
			اگر (insertion.دوسرا) {
				اگر (wire_ثنائی_ہندسہ.wire->چوڑائی == 1) {
					log_assert(wire_ثنائی_ہندسہ.انحراف == 0);
					isolated_drive_بٹ.emplace(انحراف, ثنائی_ہندسہ);
				} ورنہ
					drive_بٹ.emplace(انحراف, Drive_ثنائی_ہندسہ_Wire(wire_ثنائی_ہندسہ.wire, 0));
				اگلا_انحراف += wire_ثنائی_ہندسہ.wire->چوڑائی;
			}
			واپس insertion.پہلا->دوسرا.id + wire_ثنائی_ہندسہ.انحراف;
		}
		حالت DriveType::پورٹ_2: {
			خودکار ثابت &پورٹ_ثنائی_ہندسہ = ثنائی_ہندسہ.پورٹ();
			خودکار کلید = std::make_pair(پورٹ_ثنائی_ہندسہ.cell, پورٹ_ثنائی_ہندسہ.پورٹ);
			صحیح انحراف = اگلا_انحراف;
			خودکار insertion = پورٹ_offsets.emplace(کلید, انحراف);
			اگر (insertion.دوسرا) {
				صحیح چوڑائی = پورٹ_ثنائی_ہندسہ.cell->connections().at(پورٹ_ثنائی_ہندسہ.پورٹ).سائز();
				اگر (چوڑائی == 1 && انحراف == 0) {
					log_assert(پورٹ_ثنائی_ہندسہ.انحراف == 0);
					isolated_drive_بٹ.emplace(انحراف, ثنائی_ہندسہ);
				} ورنہ
					drive_بٹ.emplace(انحراف, Drive_ثنائی_ہندسہ_پورٹ(پورٹ_ثنائی_ہندسہ.cell, پورٹ_ثنائی_ہندسہ.پورٹ, 0));
				اگلا_انحراف += چوڑائی;
			}
			واپس insertion.پہلا->دوسرا.id + پورٹ_ثنائی_ہندسہ.انحراف;
		}
		طےشدہ:
			log_assert(جھوٹ && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

Drive_ثنائی_ہندسہ DriverMap::drive_ثنائی_ہندسہ_from_id(Drive_ثنائی_ہندسہ_Id id)
{
	خودکار ملا_isolated = isolated_drive_بٹ.ڈھونڈو(id);
	اگر (ملا_isolated != isolated_drive_بٹ.انتہا())
		واپس ملا_isolated->دوسرا;

	خودکار ملا = drive_بٹ.بالائی_bound(id);
	اگر (ملا == drive_بٹ.begin()) {
		واپس id < 0 ? Drive_ثنائی_ہندسہ() : Drive_ثنائی_ہندسہ((State) id.id);
	}
	--ملا;
	Drive_ثنائی_ہندسہ نتیجہ = ملا->دوسرا;
	اگر (نتیجہ.is_wire()) {
		نتیجہ.wire().انحراف += id.id - ملا->پہلا.id;
	} ورنہ {
		log_assert(نتیجہ.is_پورٹ());
		نتیجہ.پورٹ().انحراف += id.id - ملا->پہلا.id;
	}
	واپس نتیجہ;
}

خالی DriverMap::connect_directed_ملاؤ(Drive_ثنائی_ہندسہ_Id driven_id, Drive_ثنائی_ہندسہ_Id driver_id)
{
	اگر (driven_id == driver_id)
		واپس;

	same_driver.ملاؤ(driven_id, driver_id);

	برائے (صحیح i = 0, انتہا = connected_drivers.گنتی(driven_id); i != انتہا; ++i)
		connected_drivers.جوڑو_کنارہ(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	برائے (صحیح i = 0, انتہا = connected_undirected.گنتی(driven_id); i != انتہا; ++i)
		connected_undirected.جوڑو_کنارہ(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

خالی DriverMap::connect_directed_بفر(Drive_ثنائی_ہندسہ_Id driven_id, Drive_ثنائی_ہندسہ_Id driver_id)
{
	connected_drivers.جوڑو_کنارہ(driven_id, driver_id);
}

خالی DriverMap::connect_undirected(Drive_ثنائی_ہندسہ_Id a_id, Drive_ثنائی_ہندسہ_Id b_id)
{
	connected_undirected.جوڑو_کنارہ(a_id, b_id);
	connected_undirected.جوڑو_کنارہ(b_id, a_id);
}

خالی DriverMap::جوڑو(Module *module)
{
	برائے (خودکار ثابت &conn : module->connections())
		جوڑو(conn.پہلا, conn.دوسرا);

	برائے (خودکار cell : module->cells())
		برائے (خودکار ثابت &conn : cell->connections())
			جوڑو_پورٹ(cell, conn.پہلا, conn.دوسرا);
}

// Add a single bit connection to the driver map.
خالی DriverMap::جوڑو(Drive_ثنائی_ہندسہ ثابت &a, Drive_ثنائی_ہندسہ ثابت &b)
{
	Drive_ثنائی_ہندسہ_Id a_id = id_from_drive_ثنائی_ہندسہ(a);
	Drive_ثنائی_ہندسہ_Id b_id = id_from_drive_ثنائی_ہندسہ(b);

	Drive_ثنائی_ہندسہ_Id orig_a_id = a_id;
	Drive_ثنائی_ہندسہ_Id orig_b_id = b_id;

	a_id = same_driver.ڈھونڈو(a_id);
	b_id = same_driver.ڈھونڈو(b_id);

	اگر (a_id == b_id)
		واپس;

	ثنائی_ہندسہ_منوال a_منوال = ثنائی_ہندسہ_منوال_2(orig_a_id == a_id ? a : drive_ثنائی_ہندسہ_from_id(a_id));
	ثنائی_ہندسہ_منوال b_منوال = ثنائی_ہندسہ_منوال_2(orig_b_id == b_id ? b : drive_ثنائی_ہندسہ_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	اگر (a_منوال == ثنائی_ہندسہ_منوال::NONE && !(b_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE || b_منوال == ثنائی_ہندسہ_منوال::DRIVEN))
		connect_directed_ملاؤ(a_id, b_id);
	ورنہ اگر (b_منوال == ثنائی_ہندسہ_منوال::NONE && !(a_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE || a_منوال == ثنائی_ہندسہ_منوال::DRIVEN))
		connect_directed_ملاؤ(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	ورنہ اگر (a_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE && !(b_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE || b_منوال == ثنائی_ہندسہ_منوال::DRIVEN))
		connect_directed_بفر(a_id, b_id);
	ورنہ اگر (b_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE && !(a_منوال == ثنائی_ہندسہ_منوال::DRIVEN_UNIQUE || a_منوال == ثنائی_ہندسہ_منوال::DRIVEN))
		connect_directed_بفر(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	ورنہ اگر (a_منوال == ثنائی_ہندسہ_منوال::DRIVER)
		connect_directed_بفر(b_id, a_id);
	ورنہ اگر (b_منوال == ثنائی_ہندسہ_منوال::DRIVER)
		connect_directed_بفر(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	ورنہ
		connect_undirected(a_id, b_id);

	واپس;
}

// Specialized version that avoids unpacking
خالی DriverMap::جوڑو(SigSpec ثابت &a, SigSpec ثابت &b)
{
	log_assert(a.سائز() == b.سائز());
	خودکار ثابت &a_chunks = a.chunks();
	خودکار ثابت &b_chunks = b.chunks();

	خودکار a_chunk = a_chunks.begin();
	خودکار a_انتہا = a_chunks.انتہا();
	صحیح a_انحراف = 0;

	خودکار b_chunk = b_chunks.begin();
	صحیح b_انحراف = 0;

	SigChunk tmp_a, tmp_b;
	جب (a_chunk != a_انتہا) {
		صحیح a_چوڑائی = a_chunk->چوڑائی - a_انحراف;
		اگر (a_چوڑائی == 0) {
			a_انحراف = 0;
			++a_chunk;
			جاری;
		}
		صحیح b_چوڑائی = b_chunk->چوڑائی - b_انحراف;
		اگر (b_چوڑائی == 0) {
			b_انحراف = 0;
			++b_chunk;
			جاری;
		}
		صحیح چوڑائی = std::min(a_چوڑائی, b_چوڑائی);
		log_assert(چوڑائی > 0);

		SigChunk ثابت &a_subchunk =
			a_انحراف == 0 && a_چوڑائی == چوڑائی ? *a_chunk : a_chunk->نکالو_2(a_انحراف, چوڑائی);
		SigChunk ثابت &b_subchunk =
			b_انحراف == 0 && b_چوڑائی == چوڑائی ? *b_chunk : b_chunk->نکالو_2(b_انحراف, چوڑائی);

		جوڑو(a_subchunk, b_subchunk);

		a_انحراف += چوڑائی;
		b_انحراف += چوڑائی;
	}
}

خالی DriverMap::جوڑو_پورٹ(Cell *cell, IdString پورٹ, SigSpec ثابت &b)
{
	صحیح انحراف = 0;
	برائے (خودکار ثابت &chunk : b.chunks()) {
		جوڑو(chunk, Drive_Chunk_پورٹ(cell, پورٹ, انحراف, chunk.چوڑائی));
		انحراف += chunk.سائز();
	}
}

خالی DriverMap::orient_undirected(Drive_ثنائی_ہندسہ_Id id)
{
	pool<Drive_ثنائی_ہندسہ_Id> &نظر_آیا = orient_undirected_نظر_آیا;
	pool<Drive_ثنائی_ہندسہ_Id> &drivers = orient_undirected_drivers;
	dict<Drive_ثنائی_ہندسہ_Id, صحیح> &فاصلہ = orient_undirected_فاصلہ;
	نظر_آیا.clear();
	drivers.clear();

	نظر_آیا.emplace(id);

	برائے (صحیح pos = 0; pos < Get_سائز(نظر_آیا); ++pos) {
		Drive_ثنائی_ہندسہ_Id current = *نظر_آیا.element(نظر_آیا.سائز() - 1 - pos);
		Drive_ثنائی_ہندسہ ثنائی_ہندسہ = drive_ثنائی_ہندسہ_from_id(current);

		ثنائی_ہندسہ_منوال منوال = ثنائی_ہندسہ_منوال_2(ثنائی_ہندسہ);

		اگر (منوال == ثنائی_ہندسہ_منوال::DRIVER || منوال == ثنائی_ہندسہ_منوال::TRISTATE)
			drivers.emplace(current);

		اگر (connected_drivers.شامل_ہے(current))
			drivers.emplace(current);

		صحیح undirected_driver_گنتی = connected_undirected.گنتی(current);

		برائے (صحیح i = 0; i != undirected_driver_گنتی; ++i)
			نظر_آیا.emplace(same_driver.ڈھونڈو(connected_undirected.at(current, i)));
	}

	اگر (drivers.خالی_خانے())
		برائے (خودکار نظر_آیا_id : نظر_آیا)
			drivers.emplace(نظر_آیا_id);

	برائے (خودکار driver : drivers)
	{
		فاصلہ.clear();
		فاصلہ.emplace(driver, 0);

		برائے (صحیح pos = 0; pos < Get_سائز(فاصلہ); ++pos) {
			خودکار current_it = فاصلہ.element(فاصلہ.سائز() - 1 - pos);

			Drive_ثنائی_ہندسہ_Id current = current_it->پہلا;
			صحیح undirected_driver_گنتی = connected_undirected.گنتی(current);

			برائے (صحیح i = 0; i != undirected_driver_گنتی; ++i)
			{
				Drive_ثنائی_ہندسہ_Id اگلا = same_driver.ڈھونڈو(connected_undirected.at(current, i));
				خودکار emplaced = فاصلہ.emplace(اگلا, current_it->دوسرا + 1);
				اگر (emplaced.پہلا->دوسرا == current_it->دوسرا + 1)
					connected_oriented.جوڑو_کنارہ(اگلا, current);
			}
		}
	}

	برائے (خودکار نظر_آیا_id : نظر_آیا)
		oriented_present.emplace(نظر_آیا_id);
}

Drive_ثنائی_ہندسہ DriverMap::عامل()(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
{
	اگر (ثنائی_ہندسہ.type() == DriveType::MARKER || ثنائی_ہندسہ.type() == DriveType::NONE)
		واپس ثنائی_ہندسہ;
	اگر (ثنائی_ہندسہ.type() == DriveType::مضرب)
	{
		Drive_ثنائی_ہندسہ نتیجہ;
		برائے (خودکار ثابت &inner : ثنائی_ہندسہ.مضرب_2().مضرب_2())
			نتیجہ.ملاؤ((*یہ)(inner));
		واپس نتیجہ;
	}

	Drive_ثنائی_ہندسہ_Id ثنائی_ہندسہ_id = id_from_drive_ثنائی_ہندسہ(ثنائی_ہندسہ);

	Drive_ثنائی_ہندسہ_Id ثنائی_ہندسہ_repr_id = same_driver.ڈھونڈو(ثنائی_ہندسہ_id);

	Drive_ثنائی_ہندسہ ثنائی_ہندسہ_repr = drive_ثنائی_ہندسہ_from_id(ثنائی_ہندسہ_repr_id);

	ثنائی_ہندسہ_منوال منوال = ثنائی_ہندسہ_منوال_2(ثنائی_ہندسہ_repr);

	اگر (منوال == ثنائی_ہندسہ_منوال::KEEP && ثنائی_ہندسہ_repr_id != ثنائی_ہندسہ_id)
		واپس ثنائی_ہندسہ_repr;

	صحیح implicit_driver_گنتی = connected_drivers.گنتی(ثنائی_ہندسہ_repr_id);
	اگر (connected_undirected.شامل_ہے(ثنائی_ہندسہ_repr_id) && !oriented_present.گنتی(ثنائی_ہندسہ_repr_id))
		orient_undirected(ثنائی_ہندسہ_repr_id);

	Drive_ثنائی_ہندسہ driver;

	اگر (منوال == ثنائی_ہندسہ_منوال::DRIVER || منوال == ثنائی_ہندسہ_منوال::TRISTATE)
		driver = ثنائی_ہندسہ_repr;

	برائے (صحیح i = 0; i != implicit_driver_گنتی; ++i)
		driver.ملاؤ(drive_ثنائی_ہندسہ_from_id(connected_drivers.at(ثنائی_ہندسہ_repr_id, i)));

	صحیح oriented_driver_گنتی = connected_oriented.گنتی(ثنائی_ہندسہ_repr_id);
	برائے (صحیح i = 0; i != oriented_driver_گنتی; ++i)
		driver.ملاؤ(drive_ثنائی_ہندسہ_from_id(connected_oriented.at(ثنائی_ہندسہ_repr_id, i)));

	واپس driver;
}

DriveSpec DriverMap::عامل()(DriveSpec spec)
{
	DriveSpec نتیجہ;

	برائے (صحیح i = 0, چوڑائی = spec.سائز(); i != چوڑائی; ++i)
		نتیجہ.append((*یہ)(spec[i]));

	واپس نتیجہ;
}

std::string log_signal(DriveChunkWire ثابت &chunk)
{
	std::string id = chunk.wire->name.unescape();
	اگر (chunk.is_پورا())
		واپس id;
	اگر (chunk.چوڑائی == 1)
		واپس stringf("%s [%d]", id, chunk.انحراف);
	واپس stringf("%s [%d:%d]", id, chunk.انحراف + chunk.چوڑائی - 1, chunk.انحراف);
}


std::string log_signal(Drive_Chunk_پورٹ ثابت &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string پورٹ_id = chunk.cell->twines().unescaped_str(chunk.پورٹ);
	اگر (chunk.is_پورا())
		واپس stringf("%s <%s>", cell_id, پورٹ_id);
	اگر (chunk.چوڑائی == 1)
		واپس stringf("%s <%s> [%d]", cell_id, پورٹ_id, chunk.انحراف);
	واپس stringf("%s <%s> [%d:%d]", cell_id, پورٹ_id, chunk.انحراف + chunk.چوڑائی - 1, chunk.انحراف);
}

std::string log_signal(DriveChunkMarker ثابت &chunk)
{
	اگر (chunk.چوڑائی == 1)
		واپس stringf("<marker %d> [%d]", chunk.marker, chunk.انحراف);
	واپس stringf("<marker %d> [%d:%d]", chunk.marker, chunk.انحراف + chunk.چوڑائی - 1, chunk.انحراف);
}

std::string log_signal(DriveChunk ثابت &chunk)
{
	انتخاب (chunk.type())
	{
		حالت DriveType::NONE:
			واپس stringf("<none x%d>", chunk.سائز());
		حالت DriveType::CONSTANT:
			واپس log_const(chunk.constant());
		حالت DriveType::WIRE:
			واپس log_signal(chunk.wire());
		حالت DriveType::پورٹ_2:
			واپس log_signal(chunk.پورٹ());
		حالت DriveType::MARKER:
			واپس log_signal(chunk.marker());
		حالت DriveType::مضرب: {
			std::string str = "<multiple";
			ثابت حرف *sep = " ";
			برائے (خودکار ثابت &single : chunk.مضرب_2().مضرب_2()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			واپس str;
		}
		طےشدہ:
			log_abort();
	}
}

std::string log_signal(DriveSpec ثابت &spec)
{
	خودکار &chunks = spec.chunks();
	اگر (chunks.خالی_خانے())
		واپس "{}";
	اگر (chunks.سائز() == 1)
		واپس log_signal(chunks[0]);

	std::string str;
	ثابت حرف *sep = "{ ";

	برائے (خودکار i = chunks.rbegin(), انتہا = chunks.rend(); i != انتہا; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	واپس str;
}

YOSYS_NAMESPACE_انتہا
