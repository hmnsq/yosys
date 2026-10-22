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

DriveBit::DriveBit(SigBit ধ্রুব &bit)
{
	যদি (bit.is_wire())
		*এতত্ = DriveBitWire(bit.wire, bit.offset);
	অন্যথা
		*এতত্ = bit.data;
}

শূন্য DriveBit::merge(DriveBit ধ্রুব &other)
{
	যদি (other.type_ == DriveType::NONE)
		প্রত্যাগম;
	যদি (type_ == DriveType::NONE) {
		*এতত্ = other;
		প্রত্যাগম;
	}
	যদি (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*এতত্));
		*এতত্ = std::move(multi);
	}
	multiple().merge(other);
}


শূন্য DriveBitMultiple::merge(DriveBit ধ্রুব &single)
{
	যদি (single.type() == DriveType::NONE)
		প্রত্যাগম;
	যদি (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		প্রত্যাগম;
	}
	multiple_.emplace(single);
}

শূন্য DriveBitMultiple::merge(DriveBit &&single)
{
	যদি (single.type() == DriveType::NONE)
		প্রত্যাগম;
	যদি (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		প্রত্যাগম;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](পূর্ণাঙ্ক i) ধ্রুব
{
	DriveBitMultiple result;
	প্রতি (auto ধ্রুব &single : multiple_)
		result.merge(single[i]);
	প্রত্যাগম result;
}


তার্কিক DriveChunkWire::can_append(DriveBitWire ধ্রুব &bit) ধ্রুব
{
	প্রত্যাগম bit.wire == wire && bit.offset == offset + width;
}

তার্কিক DriveChunkWire::try_append(DriveBitWire ধ্রুব &bit)
{
	যদি (!can_append(bit))
		প্রত্যাগম অসত্য;
	width += 1;
	প্রত্যাগম সত্য;
}

তার্কিক DriveChunkWire::try_append(DriveChunkWire ধ্রুব &chunk)
{
	যদি (chunk.wire != wire || chunk.offset != offset + width)
		প্রত্যাগম অসত্য;
	width += chunk.width;
	প্রত্যাগম সত্য;
}

তার্কিক DriveChunkPort::can_append(DriveBitPort ধ্রুব &bit) ধ্রুব
{
	প্রত্যাগম bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

তার্কিক DriveChunkPort::try_append(DriveBitPort ধ্রুব &bit)
{
	যদি (!can_append(bit))
		প্রত্যাগম অসত্য;
	width += 1;
	প্রত্যাগম সত্য;
}

তার্কিক DriveChunkPort::try_append(DriveChunkPort ধ্রুব &chunk)
{
	যদি (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		প্রত্যাগম অসত্য;
	width += chunk.width;
	প্রত্যাগম সত্য;
}

তার্কিক DriveChunkMarker::can_append(DriveBitMarker ধ্রুব &bit) ধ্রুব
{
	প্রত্যাগম bit.marker == marker && bit.offset == offset + width;
}

তার্কিক DriveChunkMarker::try_append(DriveBitMarker ধ্রুব &bit)
{
	যদি (!can_append(bit))
		প্রত্যাগম অসত্য;
	width += 1;
	প্রত্যাগম সত্য;
}

তার্কিক DriveChunkMarker::try_append(DriveChunkMarker ধ্রুব &chunk)
{
	যদি (chunk.marker != marker || chunk.offset != offset + width)
		প্রত্যাগম অসত্য;
	width += chunk.width;
	প্রত্যাগম সত্য;
}


তার্কিক DriveChunkMultiple::can_append(DriveBitMultiple ধ্রুব &bit) ধ্রুব
{
	যদি (bit.multiple().size() != multiple_.size())
		প্রত্যাগম অসত্য;

	পূর্ণাঙ্ক const_drivers = 0;
	প্রতি (DriveChunk ধ্রুব &single : multiple_)
		যদি (single.is_constant())
			const_drivers += 1;

	যদি (const_drivers > 1)
		প্রত্যাগম অসত্য;

	প্রতি (DriveBit ধ্রুব &single : bit.multiple())
		যদি (single.is_constant())
			const_drivers -= 1;

	যদি (const_drivers != 0)
		প্রত্যাগম অসত্য;

	প্রতি (DriveChunk ধ্রুব &single : multiple_)
	{
		বিকল্প (single.type())
		{
			পক্ষ DriveType::CONSTANT: {
			} ভঙ্গ;
			পক্ষ DriveType::WIRE: {
				auto ধ্রুব &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				যদি (!bit.multiple().গুনতি(next))
					প্রত্যাগম অসত্য;
			} ভঙ্গ;
			পক্ষ DriveType::PORT: {
				auto ধ্রুব &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				যদি (!bit.multiple().গুনতি(next))
					প্রত্যাগম অসত্য;
			} ভঙ্গ;
			পক্ষ DriveType::MARKER: {
				auto ধ্রুব &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				যদি (!bit.multiple().গুনতি(next))
					প্রত্যাগম অসত্য;
			} ভঙ্গ;
			স্বভাব:
				প্রত্যাগম অসত্য;
		}
	}

	প্রত্যাগম সত্য;
}

তার্কিক DriveChunkMultiple::can_append(DriveChunkMultiple ধ্রুব &chunk) ধ্রুব
{
	যদি (chunk.multiple().size() != multiple_.size())
		প্রত্যাগম অসত্য;

	পূর্ণাঙ্ক const_drivers = 0;
	প্রতি (DriveChunk ধ্রুব &single : multiple_)
		যদি (single.is_constant())
			const_drivers += 1;

	যদি (const_drivers > 1)
		প্রত্যাগম অসত্য;

	প্রতি (DriveChunk ধ্রুব &single : chunk.multiple())
		যদি (single.is_constant())
			const_drivers -= 1;

	যদি (const_drivers != 0)
		প্রত্যাগম অসত্য;

	প্রতি (DriveChunk ধ্রুব &single : multiple_)
	{
		বিকল্প (single.type())
		{
			পক্ষ DriveType::CONSTANT: {
			} ভঙ্গ;
			পক্ষ DriveType::WIRE: {
				auto ধ্রুব &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				যদি (!chunk.multiple().গুনতি(next))
					প্রত্যাগম অসত্য;
			} ভঙ্গ;
			পক্ষ DriveType::PORT: {
				auto ধ্রুব &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				যদি (!chunk.multiple().গুনতি(next))
					প্রত্যাগম অসত্য;
			} ভঙ্গ;
			পক্ষ DriveType::MARKER: {
				auto ধ্রুব &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				যদি (!chunk.multiple().গুনতি(next))
					প্রত্যাগম অসত্য;
			} ভঙ্গ;
			স্বভাব:
				প্রত্যাগম অসত্য;
		}
	}

	প্রত্যাগম সত্য;
}

তার্কিক DriveChunkMultiple::try_append(DriveBitMultiple ধ্রুব &bit)
{
	যদি (!can_append(bit))
		প্রত্যাগম অসত্য;
	width_ += 1;
	State constant;

	প্রতি (DriveBit ধ্রুব &single : bit.multiple())
		যদি (single.is_constant())
			constant = single.constant();

	প্রতি (DriveChunk &single : multiple_)
	{
		বিকল্প (single.type())
		{
			পক্ষ DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} ভঙ্গ;
			পক্ষ DriveType::WIRE: {
				single.wire().width += 1;
			} ভঙ্গ;
			পক্ষ DriveType::PORT: {
				single.port().width += 1;
			} ভঙ্গ;
			পক্ষ DriveType::MARKER: {
				single.marker().width += 1;
			} ভঙ্গ;
			স্বভাব:
				log_abort();
		}
	}
	প্রত্যাগম সত্য;
}

তার্কিক DriveChunkMultiple::try_append(DriveChunkMultiple ধ্রুব &chunk)
{
	যদি (!can_append(chunk))
		প্রত্যাগম অসত্য;
	পূর্ণাঙ্ক width = chunk.size();
	width_ += width;
	Const constant;

	প্রতি (DriveChunk ধ্রুব &single : chunk.multiple())
		যদি (single.is_constant())
			constant = single.constant();

	প্রতি (DriveChunk &single : multiple_)
	{
		বিকল্প (single.type())
		{
			পক্ষ DriveType::CONSTANT: {
				single.constant().append(constant);
			} ভঙ্গ;
			পক্ষ DriveType::WIRE: {
				single.wire().width += width;
			} ভঙ্গ;
			পক্ষ DriveType::PORT: {
				single.port().width += width;
			} ভঙ্গ;
			পক্ষ DriveType::MARKER: {
				single.marker().width += width;
			} ভঙ্গ;
			স্বভাব:
				log_abort();
		}
	}
	প্রত্যাগম সত্য;
}

তার্কিক DriveChunk::can_append(DriveBit ধ্রুব &bit) ধ্রুব
{
	যদি (size() == 0)
		প্রত্যাগম সত্য;
	যদি (bit.type() != type_)
		প্রত্যাগম অসত্য;
	বিকল্প (type_)
	{
		পক্ষ DriveType::NONE:
			প্রত্যাগম সত্য;
		পক্ষ DriveType::CONSTANT:
			প্রত্যাগম সত্য;
		পক্ষ DriveType::WIRE:
			প্রত্যাগম wire_.can_append(bit.wire());
		পক্ষ DriveType::PORT:
			প্রত্যাগম port_.can_append(bit.port());
		পক্ষ DriveType::MULTIPLE:
			প্রত্যাগম multiple_.can_append(bit.multiple());
		স্বভাব:
			log_abort();
	}
}

তার্কিক DriveChunk::try_append(DriveBit ধ্রুব &bit)
{
	যদি (size() == 0)
		*এতত্ = bit;
	যদি (bit.type() != type_)
		প্রত্যাগম অসত্য;
	বিকল্প (type_)
	{
		পক্ষ DriveType::NONE:
			none_ += 1;
			প্রত্যাগম সত্য;
		পক্ষ DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			প্রত্যাগম সত্য;
		পক্ষ DriveType::WIRE:
			প্রত্যাগম wire_.try_append(bit.wire());
		পক্ষ DriveType::PORT:
			প্রত্যাগম port_.try_append(bit.port());
		পক্ষ DriveType::MULTIPLE:
			প্রত্যাগম multiple_.try_append(bit.multiple());
		স্বভাব:
			log_abort();
	}
}


তার্কিক DriveChunk::try_append(DriveChunk ধ্রুব &chunk)
{
	যদি (size() == 0)
		*এতত্ = chunk;
	যদি (chunk.type_ != type_)
		প্রত্যাগম অসত্য;
	বিকল্প (type_)
	{
		পক্ষ DriveType::NONE:
			none_ += chunk.none_;
			প্রত্যাগম সত্য;
		পক্ষ DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			প্রত্যাগম সত্য;
		পক্ষ DriveType::WIRE:
			প্রত্যাগম wire_.try_append(chunk.wire());
		পক্ষ DriveType::PORT:
			প্রত্যাগম port_.try_append(chunk.port());
		পক্ষ DriveType::MARKER:
			প্রত্যাগম marker_.try_append(chunk.marker());
		পক্ষ DriveType::MULTIPLE:
			প্রত্যাগম multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

শূন্য DriveSpec::append(DriveBit ধ্রুব &bit)
{
	hash_ = 0;
	যদি (!packed()) {
		bits_.ঠেলো_back(bit);
		width_ += 1;
		প্রত্যাগম;
	}

	যদি (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

শূন্য DriveSpec::append(DriveChunk ধ্রুব &chunk)
{
	hash_ = 0;
	pack();
	যদি (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

শূন্য DriveSpec::pack() ধ্রুব {
	যদি (bits_.empty())
		প্রত্যাগম;
	std::vector<DriveBit> bits(std::move(bits_));
	প্রতি (auto &bit : bits)
		যদি (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

শূন্য DriveSpec::unpack() ধ্রুব {
	যদি (chunks_.empty())
		প্রত্যাগম;
	প্রতি (auto &chunk : chunks_)
	{
		প্রতি (পূর্ণাঙ্ক i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

শূন্য DriveSpec::compute_width()
{
	width_ = 0;
	প্রতি (auto ধ্রুব &chunk : chunks_)
		width_ += chunk.size();
}

শূন্য DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	যদি (first_edges.emplace(src, dst).first->second == dst)
		প্রত্যাগম;
	যদি (second_edges.emplace(src, dst).first->second == dst)
		প্রত্যাগম;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::বের_করো_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	যদি (found_more != more_edges.end()) {
		auto result = found_more->second.বের_করো();
		যদি (found_more->second.empty())
			more_edges.erase(found_more);
		প্রত্যাগম result;
	}

	auto found_second = second_edges.find(src);
	যদি (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		প্রত্যাগম result;
	}

	auto found_first = first_edges.find(src);
	যদি (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		প্রত্যাগম result;
	}

	প্রত্যাগম DriveBitId();
}

শূন্য DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

তার্কিক DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	প্রত্যাগম first_edges.গুনতি(src);
}

পূর্ণাঙ্ক DriverMap::DriveBitGraph::গুনতি(DriveBitId src)
{
	যদি (!first_edges.গুনতি(src))
		প্রত্যাগম 0;
	যদি (!second_edges.গুনতি(src))
		প্রত্যাগম 1;
	auto found = more_edges.find(src);
	যদি (found == more_edges.end())
		প্রত্যাগম 2;
	প্রত্যাগম GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, পূর্ণাঙ্ক index)
{
	যদি (index == 0)
		প্রত্যাগম first_edges.at(src);
	অন্যথা যদি (index == 1)
		প্রত্যাগম second_edges.at(src);
	অন্যথা
		প্রত্যাগম *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit ধ্রুব &bit)
{
	বিকল্প (bit.type())
	{
		পক্ষ DriveType::NONE:
			প্রত্যাগম BitMode::NONE;
		পক্ষ DriveType::CONSTANT:
			// TODO how to handle Sx here?
			প্রত্যাগম bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		পক্ষ DriveType::WIRE: {
			auto ধ্রুব &wire = bit.wire();
			তার্কিক driver = wire.wire->port_প্রবেশ;
			তার্কিক driven = wire.wire->port_output;

			যদি (driver && !driven)
				প্রত্যাগম BitMode::DRIVER;
			অন্যথা যদি (driven && !driver)
				প্রত্যাগম BitMode::DRIVEN;
			অন্যথা যদি (driver && driven)
				প্রত্যাগম BitMode::TRISTATE;
			অন্যথা
				প্রত্যাগম keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		পক্ষ DriveType::PORT: {
			auto ধ্রুব &port = bit.port();
			তার্কিক driver = celltypes.cell_output(port.cell->type, port.port);
			তার্কিক driven = celltypes.cell_প্রবেশ(port.cell->type, port.port);
			যদি (driver && !driven)
				প্রত্যাগম BitMode::DRIVER;
			অন্যথা যদি (driven && !driver)
				প্রত্যাগম BitMode::DRIVEN_UNIQUE;
			অন্যথা
				প্রত্যাগম BitMode::TRISTATE;
		}
		পক্ষ DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		স্বভাব:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit ধ্রুব &bit)
{
	বিকল্প (bit.type())
	{
		পক্ষ DriveType::NONE:
			প্রত্যাগম -1;
		পক্ষ DriveType::CONSTANT:
			প্রত্যাগম (পূর্ণাঙ্ক)bit.constant();
		পক্ষ DriveType::WIRE: {
			auto ধ্রুব &wire_bit = bit.wire();
			পূর্ণাঙ্ক offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			যদি (insertion.second) {
				যদি (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} অন্যথা
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			প্রত্যাগম insertion.first->second.id + wire_bit.offset;
		}
		পক্ষ DriveType::PORT: {
			auto ধ্রুব &port_bit = bit.port();
			auto চাবি = std::make_pair(port_bit.cell, port_bit.port);
			পূর্ণাঙ্ক offset = next_offset;
			auto insertion = port_offsets.emplace(চাবি, offset);
			যদি (insertion.second) {
				পূর্ণাঙ্ক width = port_bit.cell->connections().at(port_bit.port).size();
				যদি (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} অন্যথা
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			প্রত্যাগম insertion.first->second.id + port_bit.offset;
		}
		স্বভাব:
			log_assert(অসত্য && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	যদি (found_isolated != isolated_drive_bits.end())
		প্রত্যাগম found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	যদি (found == drive_bits.begin()) {
		প্রত্যাগম id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	যদি (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} অন্যথা {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	প্রত্যাগম result;
}

শূন্য DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	যদি (driven_id == driver_id)
		প্রত্যাগম;

	same_driver.merge(driven_id, driver_id);

	প্রতি (পূর্ণাঙ্ক i = 0, end = connected_drivers.গুনতি(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	প্রতি (পূর্ণাঙ্ক i = 0, end = connected_undirected.গুনতি(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

শূন্য DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
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
	প্রতি (auto ধ্রুব &conn : module->connections())
		add(conn.first, conn.second);

	প্রতি (auto cell : module->cells())
		প্রতি (auto ধ্রুব &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
শূন্য DriverMap::add(DriveBit ধ্রুব &a, DriveBit ধ্রুব &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	যদি (a_id == b_id)
		প্রত্যাগম;

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
		connect_directed_buffer(a_id, b_id);
	অন্যথা যদি (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	অন্যথা যদি (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	অন্যথা যদি (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	অন্যথা
		connect_undirected(a_id, b_id);

	প্রত্যাগম;
}

// Specialized version that avoids unpacking
শূন্য DriverMap::add(SigSpec ধ্রুব &a, SigSpec ধ্রুব &b)
{
	log_assert(a.size() == b.size());
	auto ধ্রুব &a_chunks = a.chunks();
	auto ধ্রুব &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	পূর্ণাঙ্ক a_offset = 0;

	auto b_chunk = b_chunks.begin();
	পূর্ণাঙ্ক b_offset = 0;

	SigChunk tmp_a, tmp_b;
	যাবত্ (a_chunk != a_end) {
		পূর্ণাঙ্ক a_width = a_chunk->width - a_offset;
		যদি (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			অনুবর্ত;
		}
		পূর্ণাঙ্ক b_width = b_chunk->width - b_offset;
		যদি (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			অনুবর্ত;
		}
		পূর্ণাঙ্ক width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk ধ্রুব &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk ধ্রুব &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

শূন্য DriverMap::add_port(Cell *cell, IdString port, SigSpec ধ্রুব &b)
{
	পূর্ণাঙ্ক offset = 0;
	প্রতি (auto ধ্রুব &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

শূন্য DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, পূর্ণাঙ্ক> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	প্রতি (পূর্ণাঙ্ক pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		যদি (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		যদি (connected_drivers.contains(current))
			drivers.emplace(current);

		পূর্ণাঙ্ক undirected_driver_গুনতি = connected_undirected.গুনতি(current);

		প্রতি (পূর্ণাঙ্ক i = 0; i != undirected_driver_গুনতি; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	যদি (drivers.empty())
		প্রতি (auto seen_id : seen)
			drivers.emplace(seen_id);

	প্রতি (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		প্রতি (পূর্ণাঙ্ক pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			পূর্ণাঙ্ক undirected_driver_গুনতি = connected_undirected.গুনতি(current);

			প্রতি (পূর্ণাঙ্ক i = 0; i != undirected_driver_গুনতি; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				যদি (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	প্রতি (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit ধ্রুব &bit)
{
	যদি (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		প্রত্যাগম bit;
	যদি (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		প্রতি (auto ধ্রুব &inner : bit.multiple().multiple())
			result.merge((*এতত্)(inner));
		প্রত্যাগম result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	যদি (mode == BitMode::KEEP && bit_repr_id != bit_id)
		প্রত্যাগম bit_repr;

	পূর্ণাঙ্ক implicit_driver_গুনতি = connected_drivers.গুনতি(bit_repr_id);
	যদি (connected_undirected.contains(bit_repr_id) && !oriented_present.গুনতি(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	যদি (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	প্রতি (পূর্ণাঙ্ক i = 0; i != implicit_driver_গুনতি; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	পূর্ণাঙ্ক oriented_driver_গুনতি = connected_oriented.গুনতি(bit_repr_id);
	প্রতি (পূর্ণাঙ্ক i = 0; i != oriented_driver_গুনতি; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	প্রত্যাগম driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	প্রতি (পূর্ণাঙ্ক i = 0, width = spec.size(); i != width; ++i)
		result.append((*এতত্)(spec[i]));

	প্রত্যাগম result;
}

std::string log_signal(DriveChunkWire ধ্রুব &chunk)
{
	std::string id = chunk.wire->name.unescape();
	যদি (chunk.is_whole())
		প্রত্যাগম id;
	যদি (chunk.width == 1)
		প্রত্যাগম stringf("%s [%d]", id, chunk.offset);
	প্রত্যাগম stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort ধ্রুব &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	যদি (chunk.is_whole())
		প্রত্যাগম stringf("%s <%s>", cell_id, port_id);
	যদি (chunk.width == 1)
		প্রত্যাগম stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	প্রত্যাগম stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker ধ্রুব &chunk)
{
	যদি (chunk.width == 1)
		প্রত্যাগম stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	প্রত্যাগম stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk ধ্রুব &chunk)
{
	বিকল্প (chunk.type())
	{
		পক্ষ DriveType::NONE:
			প্রত্যাগম stringf("<none x%d>", chunk.size());
		পক্ষ DriveType::CONSTANT:
			প্রত্যাগম log_const(chunk.constant());
		পক্ষ DriveType::WIRE:
			প্রত্যাগম log_signal(chunk.wire());
		পক্ষ DriveType::PORT:
			প্রত্যাগম log_signal(chunk.port());
		পক্ষ DriveType::MARKER:
			প্রত্যাগম log_signal(chunk.marker());
		পক্ষ DriveType::MULTIPLE: {
			std::string str = "<multiple";
			ধ্রুব বর্ণ *sep = " ";
			প্রতি (auto ধ্রুব &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			প্রত্যাগম str;
		}
		স্বভাব:
			log_abort();
	}
}

std::string log_signal(DriveSpec ধ্রুব &spec)
{
	auto &chunks = spec.chunks();
	যদি (chunks.empty())
		প্রত্যাগম "{}";
	যদি (chunks.size() == 1)
		প্রত্যাগম log_signal(chunks[0]);

	std::string str;
	ধ্রুব বর্ণ *sep = "{ ";

	প্রতি (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	প্রত্যাগম str;
}

YOSYS_NAMESPACE_END
