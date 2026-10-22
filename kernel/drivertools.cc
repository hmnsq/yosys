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
	якщо (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	інакше
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	якщо (other.type_ == DriveType::NONE)
		повернути;
	якщо (type_ == DriveType::NONE) {
		*this = other;
		повернути;
	}
	якщо (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	якщо (single.type() == DriveType::NONE)
		повернути;
	якщо (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		повернути;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	якщо (single.type() == DriveType::NONE)
		повернути;
	якщо (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		повернути;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](ціле i) const
{
	DriveBitMultiple result;
	для (auto const &single : multiple_)
		result.merge(single[i]);
	повернути result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	повернути bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	якщо (!can_append(bit))
		повернути false;
	width += 1;
	повернути true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	якщо (chunk.wire != wire || chunk.offset != offset + width)
		повернути false;
	width += chunk.width;
	повернути true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	повернути bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	якщо (!can_append(bit))
		повернути false;
	width += 1;
	повернути true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	якщо (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		повернути false;
	width += chunk.width;
	повернути true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	повернути bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	якщо (!can_append(bit))
		повернути false;
	width += 1;
	повернути true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	якщо (chunk.marker != marker || chunk.offset != offset + width)
		повернути false;
	width += chunk.width;
	повернути true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	якщо (bit.multiple().size() != multiple_.size())
		повернути false;

	ціле const_drivers = 0;
	для (DriveChunk const &single : multiple_)
		якщо (single.is_constant())
			const_drivers += 1;

	якщо (const_drivers > 1)
		повернути false;

	для (DriveBit const &single : bit.multiple())
		якщо (single.is_constant())
			const_drivers -= 1;

	якщо (const_drivers != 0)
		повернути false;

	для (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
			} break;
			case DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				якщо (!bit.multiple().count(next))
					повернути false;
			} break;
			case DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				якщо (!bit.multiple().count(next))
					повернути false;
			} break;
			case DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				якщо (!bit.multiple().count(next))
					повернути false;
			} break;
			default:
				повернути false;
		}
	}

	повернути true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	якщо (chunk.multiple().size() != multiple_.size())
		повернути false;

	ціле const_drivers = 0;
	для (DriveChunk const &single : multiple_)
		якщо (single.is_constant())
			const_drivers += 1;

	якщо (const_drivers > 1)
		повернути false;

	для (DriveChunk const &single : chunk.multiple())
		якщо (single.is_constant())
			const_drivers -= 1;

	якщо (const_drivers != 0)
		повернути false;

	для (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
			} break;
			case DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				якщо (!chunk.multiple().count(next))
					повернути false;
			} break;
			case DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				якщо (!chunk.multiple().count(next))
					повернути false;
			} break;
			case DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				якщо (!chunk.multiple().count(next))
					повернути false;
			} break;
			default:
				повернути false;
		}
	}

	повернути true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	якщо (!can_append(bit))
		повернути false;
	width_ += 1;
	State constant;

	для (DriveBit const &single : bit.multiple())
		якщо (single.is_constant())
			constant = single.constant();

	для (DriveChunk &single : multiple_)
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
	повернути true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	якщо (!can_append(chunk))
		повернути false;
	ціле width = chunk.size();
	width_ += width;
	Const constant;

	для (DriveChunk const &single : chunk.multiple())
		якщо (single.is_constant())
			constant = single.constant();

	для (DriveChunk &single : multiple_)
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
	повернути true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	якщо (size() == 0)
		повернути true;
	якщо (bit.type() != type_)
		повернути false;
	switch (type_)
	{
		case DriveType::NONE:
			повернути true;
		case DriveType::CONSTANT:
			повернути true;
		case DriveType::WIRE:
			повернути wire_.can_append(bit.wire());
		case DriveType::PORT:
			повернути port_.can_append(bit.port());
		case DriveType::MULTIPLE:
			повернути multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	якщо (size() == 0)
		*this = bit;
	якщо (bit.type() != type_)
		повернути false;
	switch (type_)
	{
		case DriveType::NONE:
			none_ += 1;
			повернути true;
		case DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			повернути true;
		case DriveType::WIRE:
			повернути wire_.try_append(bit.wire());
		case DriveType::PORT:
			повернути port_.try_append(bit.port());
		case DriveType::MULTIPLE:
			повернути multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	якщо (size() == 0)
		*this = chunk;
	якщо (chunk.type_ != type_)
		повернути false;
	switch (type_)
	{
		case DriveType::NONE:
			none_ += chunk.none_;
			повернути true;
		case DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			повернути true;
		case DriveType::WIRE:
			повернути wire_.try_append(chunk.wire());
		case DriveType::PORT:
			повернути port_.try_append(chunk.port());
		case DriveType::MARKER:
			повернути marker_.try_append(chunk.marker());
		case DriveType::MULTIPLE:
			повернути multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	якщо (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		повернути;
	}

	якщо (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	якщо (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	якщо (bits_.empty())
		повернути;
	std::vector<DriveBit> bits(std::move(bits_));
	для (auto &bit : bits)
		якщо (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	якщо (chunks_.empty())
		повернути;
	для (auto &chunk : chunks_)
	{
		для (ціле i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	для (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	якщо (first_edges.emplace(src, dst).first->second == dst)
		повернути;
	якщо (second_edges.emplace(src, dst).first->second == dst)
		повернути;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	якщо (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		якщо (found_more->second.empty())
			more_edges.erase(found_more);
		повернути result;
	}

	auto found_second = second_edges.find(src);
	якщо (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		повернути result;
	}

	auto found_first = first_edges.find(src);
	якщо (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		повернути result;
	}

	повернути DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	повернути first_edges.count(src);
}

ціле DriverMap::DriveBitGraph::count(DriveBitId src)
{
	якщо (!first_edges.count(src))
		повернути 0;
	якщо (!second_edges.count(src))
		повернути 1;
	auto found = more_edges.find(src);
	якщо (found == more_edges.end())
		повернути 2;
	повернути GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, ціле index)
{
	якщо (index == 0)
		повернути first_edges.at(src);
	інакше якщо (index == 1)
		повернути second_edges.at(src);
	інакше
		повернути *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		case DriveType::NONE:
			повернути BitMode::NONE;
		case DriveType::CONSTANT:
			// TODO how to handle Sx here?
			повернути bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		case DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			якщо (driver && !driven)
				повернути BitMode::DRIVER;
			інакше якщо (driven && !driver)
				повернути BitMode::DRIVEN;
			інакше якщо (driver && driven)
				повернути BitMode::TRISTATE;
			інакше
				повернути keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		case DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			якщо (driver && !driven)
				повернути BitMode::DRIVER;
			інакше якщо (driven && !driver)
				повернути BitMode::DRIVEN_UNIQUE;
			інакше
				повернути BitMode::TRISTATE;
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
			повернути -1;
		case DriveType::CONSTANT:
			повернути (ціле)bit.constant();
		case DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			ціле offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			якщо (insertion.second) {
				якщо (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} інакше
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			повернути insertion.first->second.id + wire_bit.offset;
		}
		case DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			ціле offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			якщо (insertion.second) {
				ціле width = port_bit.cell->connections().at(port_bit.port).size();
				якщо (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} інакше
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			повернути insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	якщо (found_isolated != isolated_drive_bits.end())
		повернути found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	якщо (found == drive_bits.begin()) {
		повернути id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	якщо (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} інакше {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	повернути result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	якщо (driven_id == driver_id)
		повернути;

	same_driver.merge(driven_id, driver_id);

	для (ціле i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	для (ціле i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	для (auto const &conn : module->connections())
		add(conn.first, conn.second);

	для (auto cell : module->cells())
		для (auto const &conn : cell->connections())
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

	якщо (a_id == b_id)
		повернути;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	якщо (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	інакше якщо (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	інакше якщо (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	інакше якщо (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	інакше якщо (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	інакше якщо (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	інакше
		connect_undirected(a_id, b_id);

	повернути;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	ціле a_offset = 0;

	auto b_chunk = b_chunks.begin();
	ціле b_offset = 0;

	SigChunk tmp_a, tmp_b;
	поки (a_chunk != a_end) {
		ціле a_width = a_chunk->width - a_offset;
		якщо (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		ціле b_width = b_chunk->width - b_offset;
		якщо (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		ціле width = std::min(a_width, b_width);
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
	ціле offset = 0;
	для (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, ціле> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	для (ціле pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		якщо (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		якщо (connected_drivers.contains(current))
			drivers.emplace(current);

		ціле undirected_driver_count = connected_undirected.count(current);

		для (ціле i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	якщо (drivers.empty())
		для (auto seen_id : seen)
			drivers.emplace(seen_id);

	для (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		для (ціле pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			ціле undirected_driver_count = connected_undirected.count(current);

			для (ціле i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				якщо (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	для (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	якщо (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		повернути bit;
	якщо (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		для (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		повернути result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	якщо (mode == BitMode::KEEP && bit_repr_id != bit_id)
		повернути bit_repr;

	ціле implicit_driver_count = connected_drivers.count(bit_repr_id);
	якщо (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	якщо (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	для (ціле i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	ціле oriented_driver_count = connected_oriented.count(bit_repr_id);
	для (ціле i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	повернути driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	для (ціле i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	повернути result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	якщо (chunk.is_whole())
		повернути id;
	якщо (chunk.width == 1)
		повернути stringf("%s [%d]", id, chunk.offset);
	повернути stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	якщо (chunk.is_whole())
		повернути stringf("%s <%s>", cell_id, port_id);
	якщо (chunk.width == 1)
		повернути stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	повернути stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	якщо (chunk.width == 1)
		повернути stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	повернути stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		case DriveType::NONE:
			повернути stringf("<none x%d>", chunk.size());
		case DriveType::CONSTANT:
			повернути log_const(chunk.constant());
		case DriveType::WIRE:
			повернути log_signal(chunk.wire());
		case DriveType::PORT:
			повернути log_signal(chunk.port());
		case DriveType::MARKER:
			повернути log_signal(chunk.marker());
		case DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const char *sep = " ";
			для (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			повернути str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	якщо (chunks.empty())
		повернути "{}";
	якщо (chunks.size() == 1)
		повернути log_signal(chunks[0]);

	std::string str;
	const char *sep = "{ ";

	для (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	повернути str;
}

YOSYS_NAMESPACE_END
