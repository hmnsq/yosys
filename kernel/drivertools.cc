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

#include "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit const &bit)
{
	如果 (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	否则
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	如果 (other.type_ == DriveType::NONE)
		返回;
	如果 (type_ == DriveType::NONE) {
		*this = other;
		返回;
	}
	如果 (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	如果 (single.type() == DriveType::NONE)
		返回;
	如果 (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		返回;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	如果 (single.type() == DriveType::NONE)
		返回;
	如果 (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		返回;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](整数 i) const
{
	DriveBitMultiple result;
	为 (auto const &single : multiple_)
		result.merge(single[i]);
	返回 result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	返回 bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	如果 (!can_append(bit))
		返回 false;
	width += 1;
	返回 true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	如果 (chunk.wire != wire || chunk.offset != offset + width)
		返回 false;
	width += chunk.width;
	返回 true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	返回 bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	如果 (!can_append(bit))
		返回 false;
	width += 1;
	返回 true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	如果 (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		返回 false;
	width += chunk.width;
	返回 true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	返回 bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	如果 (!can_append(bit))
		返回 false;
	width += 1;
	返回 true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	如果 (chunk.marker != marker || chunk.offset != offset + width)
		返回 false;
	width += chunk.width;
	返回 true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	如果 (bit.multiple().size() != multiple_.size())
		返回 false;

	整数 const_drivers = 0;
	为 (DriveChunk const &single : multiple_)
		如果 (single.is_constant())
			const_drivers += 1;

	如果 (const_drivers > 1)
		返回 false;

	为 (DriveBit const &single : bit.multiple())
		如果 (single.is_constant())
			const_drivers -= 1;

	如果 (const_drivers != 0)
		返回 false;

	为 (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
			} break;
			case DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				如果 (!bit.multiple().count(next))
					返回 false;
			} break;
			case DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				如果 (!bit.multiple().count(next))
					返回 false;
			} break;
			case DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				如果 (!bit.multiple().count(next))
					返回 false;
			} break;
			default:
				返回 false;
		}
	}

	返回 true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	如果 (chunk.multiple().size() != multiple_.size())
		返回 false;

	整数 const_drivers = 0;
	为 (DriveChunk const &single : multiple_)
		如果 (single.is_constant())
			const_drivers += 1;

	如果 (const_drivers > 1)
		返回 false;

	为 (DriveChunk const &single : chunk.multiple())
		如果 (single.is_constant())
			const_drivers -= 1;

	如果 (const_drivers != 0)
		返回 false;

	为 (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
			} break;
			case DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				如果 (!chunk.multiple().count(next))
					返回 false;
			} break;
			case DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				如果 (!chunk.multiple().count(next))
					返回 false;
			} break;
			case DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				如果 (!chunk.multiple().count(next))
					返回 false;
			} break;
			default:
				返回 false;
		}
	}

	返回 true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	如果 (!can_append(bit))
		返回 false;
	width_ += 1;
	State constant;

	为 (DriveBit const &single : bit.multiple())
		如果 (single.is_constant())
			constant = single.constant();

	为 (DriveChunk &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} break;
			case DriveType::WIRE: {
				single.wire().width += 1;
			} break;
			case DriveType::PORT: {
				single.port().width += 1;
			} break;
			case DriveType::MARKER: {
				single.marker().width += 1;
			} break;
			default:
				log_abort();
		}
	}
	返回 true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	如果 (!can_append(chunk))
		返回 false;
	整数 width = chunk.size();
	width_ += width;
	Const constant;

	为 (DriveChunk const &single : chunk.multiple())
		如果 (single.is_constant())
			constant = single.constant();

	为 (DriveChunk &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
				single.constant().append(constant);
			} break;
			case DriveType::WIRE: {
				single.wire().width += width;
			} break;
			case DriveType::PORT: {
				single.port().width += width;
			} break;
			case DriveType::MARKER: {
				single.marker().width += width;
			} break;
			default:
				log_abort();
		}
	}
	返回 true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	如果 (size() == 0)
		返回 true;
	如果 (bit.type() != type_)
		返回 false;
	switch (type_)
	{
		case DriveType::NONE:
			返回 true;
		case DriveType::CONSTANT:
			返回 true;
		case DriveType::WIRE:
			返回 wire_.can_append(bit.wire());
		case DriveType::PORT:
			返回 port_.can_append(bit.port());
		case DriveType::MULTIPLE:
			返回 multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	如果 (size() == 0)
		*this = bit;
	如果 (bit.type() != type_)
		返回 false;
	switch (type_)
	{
		case DriveType::NONE:
			none_ += 1;
			返回 true;
		case DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			返回 true;
		case DriveType::WIRE:
			返回 wire_.try_append(bit.wire());
		case DriveType::PORT:
			返回 port_.try_append(bit.port());
		case DriveType::MULTIPLE:
			返回 multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	如果 (size() == 0)
		*this = chunk;
	如果 (chunk.type_ != type_)
		返回 false;
	switch (type_)
	{
		case DriveType::NONE:
			none_ += chunk.none_;
			返回 true;
		case DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			返回 true;
		case DriveType::WIRE:
			返回 wire_.try_append(chunk.wire());
		case DriveType::PORT:
			返回 port_.try_append(chunk.port());
		case DriveType::MARKER:
			返回 marker_.try_append(chunk.marker());
		case DriveType::MULTIPLE:
			返回 multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	如果 (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		返回;
	}

	如果 (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	如果 (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	如果 (bits_.empty())
		返回;
	std::vector<DriveBit> bits(std::move(bits_));
	为 (auto &bit : bits)
		如果 (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	如果 (chunks_.empty())
		返回;
	为 (auto &chunk : chunks_)
	{
		为 (整数 i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	为 (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	如果 (first_edges.emplace(src, dst).first->second == dst)
		返回;
	如果 (second_edges.emplace(src, dst).first->second == dst)
		返回;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	如果 (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		如果 (found_more->second.empty())
			more_edges.erase(found_more);
		返回 result;
	}

	auto found_second = second_edges.find(src);
	如果 (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		返回 result;
	}

	auto found_first = first_edges.find(src);
	如果 (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		返回 result;
	}

	返回 DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	返回 first_edges.count(src);
}

整数 DriverMap::DriveBitGraph::count(DriveBitId src)
{
	如果 (!first_edges.count(src))
		返回 0;
	如果 (!second_edges.count(src))
		返回 1;
	auto found = more_edges.find(src);
	如果 (found == more_edges.end())
		返回 2;
	返回 GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, 整数 index)
{
	如果 (index == 0)
		返回 first_edges.at(src);
	否则 如果 (index == 1)
		返回 second_edges.at(src);
	否则
		返回 *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		case DriveType::NONE:
			返回 BitMode::NONE;
		case DriveType::CONSTANT:
			// TODO how to handle Sx here?
			返回 bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		case DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			如果 (driver && !driven)
				返回 BitMode::DRIVER;
			否则 如果 (driven && !driver)
				返回 BitMode::DRIVEN;
			否则 如果 (driver && driven)
				返回 BitMode::TRISTATE;
			否则
				返回 keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		case DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			如果 (driver && !driven)
				返回 BitMode::DRIVER;
			否则 如果 (driven && !driver)
				返回 BitMode::DRIVEN_UNIQUE;
			否则
				返回 BitMode::TRISTATE;
		}
		case DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		default:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit const &bit)
{
	switch (bit.type())
	{
		case DriveType::NONE:
			返回 -1;
		case DriveType::CONSTANT:
			返回 (整数)bit.constant();
		case DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			整数 offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			如果 (insertion.second) {
				如果 (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} 否则
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			返回 insertion.first->second.id + wire_bit.offset;
		}
		case DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			整数 offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			如果 (insertion.second) {
				整数 width = port_bit.cell->connections().at(port_bit.port).size();
				如果 (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} 否则
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			返回 insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	如果 (found_isolated != isolated_drive_bits.end())
		返回 found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	如果 (found == drive_bits.begin()) {
		返回 id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	如果 (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} 否则 {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	返回 result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	如果 (driven_id == driver_id)
		返回;

	same_driver.merge(driven_id, driver_id);

	为 (整数 i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	为 (整数 i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

void DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

void DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

void DriverMap::add(Module *module)
{
	为 (auto const &conn : module->connections())
		add(conn.first, conn.second);

	为 (auto cell : module->cells())
		为 (auto const &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
void DriverMap::add(DriveBit const &a, DriveBit const &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	如果 (a_id == b_id)
		返回;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	如果 (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	否则 如果 (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	否则 如果 (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	否则 如果 (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	否则 如果 (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	否则 如果 (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	否则
		connect_undirected(a_id, b_id);

	返回;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	整数 a_offset = 0;

	auto b_chunk = b_chunks.begin();
	整数 b_offset = 0;

	SigChunk tmp_a, tmp_b;
	当 (a_chunk != a_end) {
		整数 a_width = a_chunk->width - a_offset;
		如果 (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		整数 b_width = b_chunk->width - b_offset;
		如果 (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		整数 width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk const &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk const &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

void DriverMap::add_port(Cell *cell, IdString port, SigSpec const &b)
{
	整数 offset = 0;
	为 (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, 整数> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	为 (整数 pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		如果 (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		如果 (connected_drivers.contains(current))
			drivers.emplace(current);

		整数 undirected_driver_count = connected_undirected.count(current);

		为 (整数 i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	如果 (drivers.empty())
		为 (auto seen_id : seen)
			drivers.emplace(seen_id);

	为 (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		为 (整数 pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			整数 undirected_driver_count = connected_undirected.count(current);

			为 (整数 i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				如果 (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	为 (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	如果 (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		返回 bit;
	如果 (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		为 (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		返回 result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	如果 (mode == BitMode::KEEP && bit_repr_id != bit_id)
		返回 bit_repr;

	整数 implicit_driver_count = connected_drivers.count(bit_repr_id);
	如果 (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	如果 (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	为 (整数 i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	整数 oriented_driver_count = connected_oriented.count(bit_repr_id);
	为 (整数 i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	返回 driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	为 (整数 i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	返回 result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	如果 (chunk.is_whole())
		返回 id;
	如果 (chunk.width == 1)
		返回 stringf("%s [%d]", id, chunk.offset);
	返回 stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	如果 (chunk.is_whole())
		返回 stringf("%s <%s>", cell_id, port_id);
	如果 (chunk.width == 1)
		返回 stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	返回 stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	如果 (chunk.width == 1)
		返回 stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	返回 stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		case DriveType::NONE:
			返回 stringf("<none x%d>", chunk.size());
		case DriveType::CONSTANT:
			返回 log_const(chunk.constant());
		case DriveType::WIRE:
			返回 log_signal(chunk.wire());
		case DriveType::PORT:
			返回 log_signal(chunk.port());
		case DriveType::MARKER:
			返回 log_signal(chunk.marker());
		case DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const char *sep = " ";
			为 (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			返回 str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	如果 (chunks.empty())
		返回 "{}";
	如果 (chunks.size() == 1)
		返回 log_signal(chunks[0]);

	std::string str;
	const char *sep = "{ ";

	为 (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	返回 str;
}

YOSYS_NAMESPACE_END
