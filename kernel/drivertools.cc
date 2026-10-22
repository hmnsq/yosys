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

#অন্তর্ভাব "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit ধ্রুবক &bit)
{
	যদি (bit.is_wire())
		*এই = DriveBitWire(bit.wire, bit.offset);
	অন্যথা
		*এই = bit.তথ্য;
}

শূন্য DriveBit::merge(DriveBit ধ্রুবক &other)
{
	যদি (other.type_ == DriveType::NONE)
		ফেরত;
	যদি (type_ == DriveType::NONE) {
		*এই = other;
		ফেরত;
	}
	যদি (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*এই));
		*এই = std::move(multi);
	}
	multiple().merge(other);
}


শূন্য DriveBitMultiple::merge(DriveBit ধ্রুবক &single)
{
	যদি (single.type() == DriveType::NONE)
		ফেরত;
	যদি (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ফেরত;
	}
	multiple_.emplace(single);
}

শূন্য DriveBitMultiple::merge(DriveBit &&single)
{
	যদি (single.type() == DriveType::NONE)
		ফেরত;
	যদি (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ফেরত;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](পূর্ণসংখ্যা i) ধ্রুবক
{
	DriveBitMultiple ফলাফল;
	জন্য (auto ধ্রুবক &single : multiple_)
		ফলাফল.merge(single[i]);
	ফেরত ফলাফল;
}


যৌক্তিক DriveChunkWire::can_append(DriveBitWire ধ্রুবক &bit) ধ্রুবক
{
	ফেরত bit.wire == wire && bit.offset == offset + width;
}

যৌক্তিক DriveChunkWire::try_append(DriveBitWire ধ্রুবক &bit)
{
	যদি (!can_append(bit))
		ফেরত মিথ্যা;
	width += 1;
	ফেরত সত্য;
}

যৌক্তিক DriveChunkWire::try_append(DriveChunkWire ধ্রুবক &chunk)
{
	যদি (chunk.wire != wire || chunk.offset != offset + width)
		ফেরত মিথ্যা;
	width += chunk.width;
	ফেরত সত্য;
}

যৌক্তিক DriveChunkPort::can_append(DriveBitPort ধ্রুবক &bit) ধ্রুবক
{
	ফেরত bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

যৌক্তিক DriveChunkPort::try_append(DriveBitPort ধ্রুবক &bit)
{
	যদি (!can_append(bit))
		ফেরত মিথ্যা;
	width += 1;
	ফেরত সত্য;
}

যৌক্তিক DriveChunkPort::try_append(DriveChunkPort ধ্রুবক &chunk)
{
	যদি (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ফেরত মিথ্যা;
	width += chunk.width;
	ফেরত সত্য;
}

যৌক্তিক DriveChunkMarker::can_append(DriveBitMarker ধ্রুবক &bit) ধ্রুবক
{
	ফেরত bit.marker == marker && bit.offset == offset + width;
}

যৌক্তিক DriveChunkMarker::try_append(DriveBitMarker ধ্রুবক &bit)
{
	যদি (!can_append(bit))
		ফেরত মিথ্যা;
	width += 1;
	ফেরত সত্য;
}

যৌক্তিক DriveChunkMarker::try_append(DriveChunkMarker ধ্রুবক &chunk)
{
	যদি (chunk.marker != marker || chunk.offset != offset + width)
		ফেরত মিথ্যা;
	width += chunk.width;
	ফেরত সত্য;
}


যৌক্তিক DriveChunkMultiple::can_append(DriveBitMultiple ধ্রুবক &bit) ধ্রুবক
{
	যদি (bit.multiple().আকার_2() != multiple_.আকার_2())
		ফেরত মিথ্যা;

	পূর্ণসংখ্যা const_drivers = 0;
	জন্য (DriveChunk ধ্রুবক &single : multiple_)
		যদি (single.is_constant())
			const_drivers += 1;

	যদি (const_drivers > 1)
		ফেরত মিথ্যা;

	জন্য (DriveBit ধ্রুবক &single : bit.multiple())
		যদি (single.is_constant())
			const_drivers -= 1;

	যদি (const_drivers != 0)
		ফেরত মিথ্যা;

	জন্য (DriveChunk ধ্রুবক &single : multiple_)
	{
		নির্বাচন (single.type())
		{
			ক্ষেত্র DriveType::CONSTANT: {
			} বিরতি;
			ক্ষেত্র DriveType::WIRE: {
				auto ধ্রুবক &wire = single.wire();
				DriveBit পরবর্তী = DriveBitWire(wire.wire, wire.offset + wire.width);
				যদি (!bit.multiple().গুনতি(পরবর্তী))
					ফেরত মিথ্যা;
			} বিরতি;
			ক্ষেত্র DriveType::PORT: {
				auto ধ্রুবক &port = single.port();
				DriveBit পরবর্তী = DriveBitPort(port.cell, port.port, port.offset + port.width);
				যদি (!bit.multiple().গুনতি(পরবর্তী))
					ফেরত মিথ্যা;
			} বিরতি;
			ক্ষেত্র DriveType::MARKER: {
				auto ধ্রুবক &marker = single.marker();
				DriveBit পরবর্তী = DriveBitMarker(marker.marker, marker.offset + marker.width);
				যদি (!bit.multiple().গুনতি(পরবর্তী))
					ফেরত মিথ্যা;
			} বিরতি;
			পূর্বনির্ধারিত:
				ফেরত মিথ্যা;
		}
	}

	ফেরত সত্য;
}

যৌক্তিক DriveChunkMultiple::can_append(DriveChunkMultiple ধ্রুবক &chunk) ধ্রুবক
{
	যদি (chunk.multiple().আকার_2() != multiple_.আকার_2())
		ফেরত মিথ্যা;

	পূর্ণসংখ্যা const_drivers = 0;
	জন্য (DriveChunk ধ্রুবক &single : multiple_)
		যদি (single.is_constant())
			const_drivers += 1;

	যদি (const_drivers > 1)
		ফেরত মিথ্যা;

	জন্য (DriveChunk ধ্রুবক &single : chunk.multiple())
		যদি (single.is_constant())
			const_drivers -= 1;

	যদি (const_drivers != 0)
		ফেরত মিথ্যা;

	জন্য (DriveChunk ধ্রুবক &single : multiple_)
	{
		নির্বাচন (single.type())
		{
			ক্ষেত্র DriveType::CONSTANT: {
			} বিরতি;
			ক্ষেত্র DriveType::WIRE: {
				auto ধ্রুবক &wire = single.wire();
				DriveChunk পরবর্তী = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.আকার_2());
				যদি (!chunk.multiple().গুনতি(পরবর্তী))
					ফেরত মিথ্যা;
			} বিরতি;
			ক্ষেত্র DriveType::PORT: {
				auto ধ্রুবক &port = single.port();
				DriveChunk পরবর্তী = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.আকার_2());
				যদি (!chunk.multiple().গুনতি(পরবর্তী))
					ফেরত মিথ্যা;
			} বিরতি;
			ক্ষেত্র DriveType::MARKER: {
				auto ধ্রুবক &marker = single.marker();
				DriveChunk পরবর্তী = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.আকার_2());
				যদি (!chunk.multiple().গুনতি(পরবর্তী))
					ফেরত মিথ্যা;
			} বিরতি;
			পূর্বনির্ধারিত:
				ফেরত মিথ্যা;
		}
	}

	ফেরত সত্য;
}

যৌক্তিক DriveChunkMultiple::try_append(DriveBitMultiple ধ্রুবক &bit)
{
	যদি (!can_append(bit))
		ফেরত মিথ্যা;
	width_ += 1;
	State constant;

	জন্য (DriveBit ধ্রুবক &single : bit.multiple())
		যদি (single.is_constant())
			constant = single.constant();

	জন্য (DriveChunk &single : multiple_)
	{
		নির্বাচন (single.type())
		{
			ক্ষেত্র DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} বিরতি;
			ক্ষেত্র DriveType::WIRE: {
				single.wire().width += 1;
			} বিরতি;
			ক্ষেত্র DriveType::PORT: {
				single.port().width += 1;
			} বিরতি;
			ক্ষেত্র DriveType::MARKER: {
				single.marker().width += 1;
			} বিরতি;
			পূর্বনির্ধারিত:
				log_abort();
		}
	}
	ফেরত সত্য;
}

যৌক্তিক DriveChunkMultiple::try_append(DriveChunkMultiple ধ্রুবক &chunk)
{
	যদি (!can_append(chunk))
		ফেরত মিথ্যা;
	পূর্ণসংখ্যা width = chunk.আকার_2();
	width_ += width;
	Const constant;

	জন্য (DriveChunk ধ্রুবক &single : chunk.multiple())
		যদি (single.is_constant())
			constant = single.constant();

	জন্য (DriveChunk &single : multiple_)
	{
		নির্বাচন (single.type())
		{
			ক্ষেত্র DriveType::CONSTANT: {
				single.constant().append(constant);
			} বিরতি;
			ক্ষেত্র DriveType::WIRE: {
				single.wire().width += width;
			} বিরতি;
			ক্ষেত্র DriveType::PORT: {
				single.port().width += width;
			} বিরতি;
			ক্ষেত্র DriveType::MARKER: {
				single.marker().width += width;
			} বিরতি;
			পূর্বনির্ধারিত:
				log_abort();
		}
	}
	ফেরত সত্য;
}

যৌক্তিক DriveChunk::can_append(DriveBit ধ্রুবক &bit) ধ্রুবক
{
	যদি (আকার_2() == 0)
		ফেরত সত্য;
	যদি (bit.type() != type_)
		ফেরত মিথ্যা;
	নির্বাচন (type_)
	{
		ক্ষেত্র DriveType::NONE:
			ফেরত সত্য;
		ক্ষেত্র DriveType::CONSTANT:
			ফেরত সত্য;
		ক্ষেত্র DriveType::WIRE:
			ফেরত wire_.can_append(bit.wire());
		ক্ষেত্র DriveType::PORT:
			ফেরত port_.can_append(bit.port());
		ক্ষেত্র DriveType::MULTIPLE:
			ফেরত multiple_.can_append(bit.multiple());
		পূর্বনির্ধারিত:
			log_abort();
	}
}

যৌক্তিক DriveChunk::try_append(DriveBit ধ্রুবক &bit)
{
	যদি (আকার_2() == 0)
		*এই = bit;
	যদি (bit.type() != type_)
		ফেরত মিথ্যা;
	নির্বাচন (type_)
	{
		ক্ষেত্র DriveType::NONE:
			none_ += 1;
			ফেরত সত্য;
		ক্ষেত্র DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ফেরত সত্য;
		ক্ষেত্র DriveType::WIRE:
			ফেরত wire_.try_append(bit.wire());
		ক্ষেত্র DriveType::PORT:
			ফেরত port_.try_append(bit.port());
		ক্ষেত্র DriveType::MULTIPLE:
			ফেরত multiple_.try_append(bit.multiple());
		পূর্বনির্ধারিত:
			log_abort();
	}
}


যৌক্তিক DriveChunk::try_append(DriveChunk ধ্রুবক &chunk)
{
	যদি (আকার_2() == 0)
		*এই = chunk;
	যদি (chunk.type_ != type_)
		ফেরত মিথ্যা;
	নির্বাচন (type_)
	{
		ক্ষেত্র DriveType::NONE:
			none_ += chunk.none_;
			ফেরত সত্য;
		ক্ষেত্র DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ফেরত সত্য;
		ক্ষেত্র DriveType::WIRE:
			ফেরত wire_.try_append(chunk.wire());
		ক্ষেত্র DriveType::PORT:
			ফেরত port_.try_append(chunk.port());
		ক্ষেত্র DriveType::MARKER:
			ফেরত marker_.try_append(chunk.marker());
		ক্ষেত্র DriveType::MULTIPLE:
			ফেরত multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

শূন্য DriveSpec::append(DriveBit ধ্রুবক &bit)
{
	hash_ = 0;
	যদি (!packed()) {
		bits_.ঠেলো_back(bit);
		width_ += 1;
		ফেরত;
	}

	যদি (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

শূন্য DriveSpec::append(DriveChunk ধ্রুবক &chunk)
{
	hash_ = 0;
	pack();
	যদি (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.আকার_2();
}

শূন্য DriveSpec::pack() ধ্রুবক {
	যদি (bits_.empty())
		ফেরত;
	std::vector<DriveBit> bits(std::move(bits_));
	জন্য (auto &bit : bits)
		যদি (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

শূন্য DriveSpec::unpack() ধ্রুবক {
	যদি (chunks_.empty())
		ফেরত;
	জন্য (auto &chunk : chunks_)
	{
		জন্য (পূর্ণসংখ্যা i = 0, width = chunk.আকার_2(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

শূন্য DriveSpec::compute_width()
{
	width_ = 0;
	জন্য (auto ধ্রুবক &chunk : chunks_)
		width_ += chunk.আকার_2();
}

শূন্য DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	যদি (first_edges.emplace(src, dst).first->second == dst)
		ফেরত;
	যদি (second_edges.emplace(src, dst).first->second == dst)
		ফেরত;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::বের_করো_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	যদি (found_more != more_edges.end()) {
		auto ফলাফল = found_more->second.বের_করো();
		যদি (found_more->second.empty())
			more_edges.erase(found_more);
		ফেরত ফলাফল;
	}

	auto found_second = second_edges.find(src);
	যদি (found_second != second_edges.end()) {
		auto ফলাফল = found_second->second;
		second_edges.erase(found_second);
		ফেরত ফলাফল;
	}

	auto found_first = first_edges.find(src);
	যদি (found_first != first_edges.end()) {
		auto ফলাফল = found_first->second;
		first_edges.erase(found_first);
		ফেরত ফলাফল;
	}

	ফেরত DriveBitId();
}

শূন্য DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

যৌক্তিক DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ফেরত first_edges.গুনতি(src);
}

পূর্ণসংখ্যা DriverMap::DriveBitGraph::গুনতি(DriveBitId src)
{
	যদি (!first_edges.গুনতি(src))
		ফেরত 0;
	যদি (!second_edges.গুনতি(src))
		ফেরত 1;
	auto found = more_edges.find(src);
	যদি (found == more_edges.end())
		ফেরত 2;
	ফেরত Get_আকার(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, পূর্ণসংখ্যা সূচক)
{
	যদি (সূচক == 0)
		ফেরত first_edges.at(src);
	অন্যথা যদি (সূচক == 1)
		ফেরত second_edges.at(src);
	অন্যথা
		ফেরত *more_edges.at(src).element(সূচক - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit ধ্রুবক &bit)
{
	নির্বাচন (bit.type())
	{
		ক্ষেত্র DriveType::NONE:
			ফেরত BitMode::NONE;
		ক্ষেত্র DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ফেরত bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		ক্ষেত্র DriveType::WIRE: {
			auto ধ্রুবক &wire = bit.wire();
			যৌক্তিক driver = wire.wire->port_প্রবেশ;
			যৌক্তিক driven = wire.wire->port_output;

			যদি (driver && !driven)
				ফেরত BitMode::DRIVER;
			অন্যথা যদি (driven && !driver)
				ফেরত BitMode::DRIVEN;
			অন্যথা যদি (driver && driven)
				ফেরত BitMode::TRISTATE;
			অন্যথা
				ফেরত keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		ক্ষেত্র DriveType::PORT: {
			auto ধ্রুবক &port = bit.port();
			যৌক্তিক driver = celltypes.cell_output(port.cell->type, port.port);
			যৌক্তিক driven = celltypes.cell_প্রবেশ(port.cell->type, port.port);
			যদি (driver && !driven)
				ফেরত BitMode::DRIVER;
			অন্যথা যদি (driven && !driver)
				ফেরত BitMode::DRIVEN_UNIQUE;
			অন্যথা
				ফেরত BitMode::TRISTATE;
		}
		ক্ষেত্র DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		পূর্বনির্ধারিত:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit ধ্রুবক &bit)
{
	নির্বাচন (bit.type())
	{
		ক্ষেত্র DriveType::NONE:
			ফেরত -1;
		ক্ষেত্র DriveType::CONSTANT:
			ফেরত (পূর্ণসংখ্যা)bit.constant();
		ক্ষেত্র DriveType::WIRE: {
			auto ধ্রুবক &wire_bit = bit.wire();
			পূর্ণসংখ্যা offset = পরবর্তী_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			যদি (insertion.second) {
				যদি (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} অন্যথা
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				পরবর্তী_offset += wire_bit.wire->width;
			}
			ফেরত insertion.first->second.id + wire_bit.offset;
		}
		ক্ষেত্র DriveType::PORT: {
			auto ধ্রুবক &port_bit = bit.port();
			auto চাবি = std::make_pair(port_bit.cell, port_bit.port);
			পূর্ণসংখ্যা offset = পরবর্তী_offset;
			auto insertion = port_offsets.emplace(চাবি, offset);
			যদি (insertion.second) {
				পূর্ণসংখ্যা width = port_bit.cell->connections().at(port_bit.port).আকার_2();
				যদি (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} অন্যথা
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				পরবর্তী_offset += width;
			}
			ফেরত insertion.first->second.id + port_bit.offset;
		}
		পূর্বনির্ধারিত:
			log_assert(মিথ্যা && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	যদি (found_isolated != isolated_drive_bits.end())
		ফেরত found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	যদি (found == drive_bits.begin()) {
		ফেরত id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit ফলাফল = found->second;
	যদি (ফলাফল.is_wire()) {
		ফলাফল.wire().offset += id.id - found->first.id;
	} অন্যথা {
		log_assert(ফলাফল.is_port());
		ফলাফল.port().offset += id.id - found->first.id;
	}
	ফেরত ফলাফল;
}

শূন্য DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	যদি (driven_id == driver_id)
		ফেরত;

	same_driver.merge(driven_id, driver_id);

	জন্য (পূর্ণসংখ্যা i = 0, end = connected_drivers.গুনতি(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	জন্য (পূর্ণসংখ্যা i = 0, end = connected_undirected.গুনতি(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

শূন্য DriverMap::connect_directed_বাফার(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

শূন্য DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

শূন্য DriverMap::add(Module *module)
{
	জন্য (auto ধ্রুবক &conn : module->connections())
		add(conn.first, conn.second);

	জন্য (auto cell : module->cells())
		জন্য (auto ধ্রুবক &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
শূন্য DriverMap::add(DriveBit ধ্রুবক &a, DriveBit ধ্রুবক &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	যদি (a_id == b_id)
		ফেরত;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	যদি (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	অন্যথা যদি (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	অন্যথা যদি (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_বাফার(a_id, b_id);
	অন্যথা যদি (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_বাফার(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	অন্যথা যদি (a_mode == BitMode::DRIVER)
		connect_directed_বাফার(b_id, a_id);
	অন্যথা যদি (b_mode == BitMode::DRIVER)
		connect_directed_বাফার(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	অন্যথা
		connect_undirected(a_id, b_id);

	ফেরত;
}

// Specialized version that avoids unpacking
শূন্য DriverMap::add(SigSpec ধ্রুবক &a, SigSpec ধ্রুবক &b)
{
	log_assert(a.আকার_2() == b.আকার_2());
	auto ধ্রুবক &a_chunks = a.chunks();
	auto ধ্রুবক &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	পূর্ণসংখ্যা a_offset = 0;

	auto b_chunk = b_chunks.begin();
	পূর্ণসংখ্যা b_offset = 0;

	SigChunk tmp_a, tmp_b;
	যখন (a_chunk != a_end) {
		পূর্ণসংখ্যা a_width = a_chunk->width - a_offset;
		যদি (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			চালু;
		}
		পূর্ণসংখ্যা b_width = b_chunk->width - b_offset;
		যদি (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			চালু;
		}
		পূর্ণসংখ্যা width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk ধ্রুবক &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk ধ্রুবক &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

শূন্য DriverMap::add_port(Cell *cell, IdString port, SigSpec ধ্রুবক &b)
{
	পূর্ণসংখ্যা offset = 0;
	জন্য (auto ধ্রুবক &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.আকার_2();
	}
}

শূন্য DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, পূর্ণসংখ্যা> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	জন্য (পূর্ণসংখ্যা pos = 0; pos < Get_আকার(seen); ++pos) {
		DriveBitId current = *seen.element(seen.আকার_2() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		যদি (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		যদি (connected_drivers.contains(current))
			drivers.emplace(current);

		পূর্ণসংখ্যা undirected_driver_গুনতি = connected_undirected.গুনতি(current);

		জন্য (পূর্ণসংখ্যা i = 0; i != undirected_driver_গুনতি; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	যদি (drivers.empty())
		জন্য (auto seen_id : seen)
			drivers.emplace(seen_id);

	জন্য (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		জন্য (পূর্ণসংখ্যা pos = 0; pos < Get_আকার(distance); ++pos) {
			auto current_it = distance.element(distance.আকার_2() - 1 - pos);

			DriveBitId current = current_it->first;
			পূর্ণসংখ্যা undirected_driver_গুনতি = connected_undirected.গুনতি(current);

			জন্য (পূর্ণসংখ্যা i = 0; i != undirected_driver_গুনতি; ++i)
			{
				DriveBitId পরবর্তী = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(পরবর্তী, current_it->second + 1);
				যদি (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(পরবর্তী, current);
			}
		}
	}

	জন্য (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit ধ্রুবক &bit)
{
	যদি (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ফেরত bit;
	যদি (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit ফলাফল;
		জন্য (auto ধ্রুবক &inner : bit.multiple().multiple())
			ফলাফল.merge((*এই)(inner));
		ফেরত ফলাফল;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	যদি (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ফেরত bit_repr;

	পূর্ণসংখ্যা implicit_driver_গুনতি = connected_drivers.গুনতি(bit_repr_id);
	যদি (connected_undirected.contains(bit_repr_id) && !oriented_present.গুনতি(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	যদি (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	জন্য (পূর্ণসংখ্যা i = 0; i != implicit_driver_গুনতি; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	পূর্ণসংখ্যা oriented_driver_গুনতি = connected_oriented.গুনতি(bit_repr_id);
	জন্য (পূর্ণসংখ্যা i = 0; i != oriented_driver_গুনতি; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ফেরত driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec ফলাফল;

	জন্য (পূর্ণসংখ্যা i = 0, width = spec.আকার_2(); i != width; ++i)
		ফলাফল.append((*এই)(spec[i]));

	ফেরত ফলাফল;
}

std::string log_signal(DriveChunkWire ধ্রুবক &chunk)
{
	std::string id = chunk.wire->name.unescape();
	যদি (chunk.is_whole())
		ফেরত id;
	যদি (chunk.width == 1)
		ফেরত stringf("%s [%d]", id, chunk.offset);
	ফেরত stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort ধ্রুবক &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	যদি (chunk.is_whole())
		ফেরত stringf("%s <%s>", cell_id, port_id);
	যদি (chunk.width == 1)
		ফেরত stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ফেরত stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker ধ্রুবক &chunk)
{
	যদি (chunk.width == 1)
		ফেরত stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ফেরত stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk ধ্রুবক &chunk)
{
	নির্বাচন (chunk.type())
	{
		ক্ষেত্র DriveType::NONE:
			ফেরত stringf("<none x%d>", chunk.আকার_2());
		ক্ষেত্র DriveType::CONSTANT:
			ফেরত log_const(chunk.constant());
		ক্ষেত্র DriveType::WIRE:
			ফেরত log_signal(chunk.wire());
		ক্ষেত্র DriveType::PORT:
			ফেরত log_signal(chunk.port());
		ক্ষেত্র DriveType::MARKER:
			ফেরত log_signal(chunk.marker());
		ক্ষেত্র DriveType::MULTIPLE: {
			std::string str = "<multiple";
			ধ্রুবক অক্ষর *sep = " ";
			জন্য (auto ধ্রুবক &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ফেরত str;
		}
		পূর্বনির্ধারিত:
			log_abort();
	}
}

std::string log_signal(DriveSpec ধ্রুবক &spec)
{
	auto &chunks = spec.chunks();
	যদি (chunks.empty())
		ফেরত "{}";
	যদি (chunks.আকার_2() == 1)
		ফেরত log_signal(chunks[0]);

	std::string str;
	ধ্রুবক অক্ষর *sep = "{ ";

	জন্য (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ফেরত str;
}

YOSYS_NAMESPACE_END
