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
	nếu (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	không_thì
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	nếu (other.type_ == DriveType::NONE)
		trả_về;
	nếu (type_ == DriveType::NONE) {
		*this = other;
		trả_về;
	}
	nếu (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	nếu (single.type() == DriveType::NONE)
		trả_về;
	nếu (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		trả_về;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	nếu (single.type() == DriveType::NONE)
		trả_về;
	nếu (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		trả_về;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](số_nguyên i) const
{
	DriveBitMultiple result;
	cho (auto const &single : multiple_)
		result.merge(single[i]);
	trả_về result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	trả_về bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	nếu (!can_append(bit))
		trả_về false;
	width += 1;
	trả_về true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	nếu (chunk.wire != wire || chunk.offset != offset + width)
		trả_về false;
	width += chunk.width;
	trả_về true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	trả_về bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	nếu (!can_append(bit))
		trả_về false;
	width += 1;
	trả_về true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	nếu (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		trả_về false;
	width += chunk.width;
	trả_về true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	trả_về bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	nếu (!can_append(bit))
		trả_về false;
	width += 1;
	trả_về true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	nếu (chunk.marker != marker || chunk.offset != offset + width)
		trả_về false;
	width += chunk.width;
	trả_về true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	nếu (bit.multiple().size() != multiple_.size())
		trả_về false;

	số_nguyên const_drivers = 0;
	cho (DriveChunk const &single : multiple_)
		nếu (single.is_constant())
			const_drivers += 1;

	nếu (const_drivers > 1)
		trả_về false;

	cho (DriveBit const &single : bit.multiple())
		nếu (single.is_constant())
			const_drivers -= 1;

	nếu (const_drivers != 0)
		trả_về false;

	cho (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
			} break;
			case DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				nếu (!bit.multiple().count(next))
					trả_về false;
			} break;
			case DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				nếu (!bit.multiple().count(next))
					trả_về false;
			} break;
			case DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				nếu (!bit.multiple().count(next))
					trả_về false;
			} break;
			default:
				trả_về false;
		}
	}

	trả_về true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	nếu (chunk.multiple().size() != multiple_.size())
		trả_về false;

	số_nguyên const_drivers = 0;
	cho (DriveChunk const &single : multiple_)
		nếu (single.is_constant())
			const_drivers += 1;

	nếu (const_drivers > 1)
		trả_về false;

	cho (DriveChunk const &single : chunk.multiple())
		nếu (single.is_constant())
			const_drivers -= 1;

	nếu (const_drivers != 0)
		trả_về false;

	cho (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
			} break;
			case DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				nếu (!chunk.multiple().count(next))
					trả_về false;
			} break;
			case DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				nếu (!chunk.multiple().count(next))
					trả_về false;
			} break;
			case DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				nếu (!chunk.multiple().count(next))
					trả_về false;
			} break;
			default:
				trả_về false;
		}
	}

	trả_về true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	nếu (!can_append(bit))
		trả_về false;
	width_ += 1;
	State constant;

	cho (DriveBit const &single : bit.multiple())
		nếu (single.is_constant())
			constant = single.constant();

	cho (DriveChunk &single : multiple_)
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
	trả_về true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	nếu (!can_append(chunk))
		trả_về false;
	số_nguyên width = chunk.size();
	width_ += width;
	Const constant;

	cho (DriveChunk const &single : chunk.multiple())
		nếu (single.is_constant())
			constant = single.constant();

	cho (DriveChunk &single : multiple_)
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
	trả_về true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	nếu (size() == 0)
		trả_về true;
	nếu (bit.type() != type_)
		trả_về false;
	switch (type_)
	{
		case DriveType::NONE:
			trả_về true;
		case DriveType::CONSTANT:
			trả_về true;
		case DriveType::WIRE:
			trả_về wire_.can_append(bit.wire());
		case DriveType::PORT:
			trả_về port_.can_append(bit.port());
		case DriveType::MULTIPLE:
			trả_về multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	nếu (size() == 0)
		*this = bit;
	nếu (bit.type() != type_)
		trả_về false;
	switch (type_)
	{
		case DriveType::NONE:
			none_ += 1;
			trả_về true;
		case DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			trả_về true;
		case DriveType::WIRE:
			trả_về wire_.try_append(bit.wire());
		case DriveType::PORT:
			trả_về port_.try_append(bit.port());
		case DriveType::MULTIPLE:
			trả_về multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	nếu (size() == 0)
		*this = chunk;
	nếu (chunk.type_ != type_)
		trả_về false;
	switch (type_)
	{
		case DriveType::NONE:
			none_ += chunk.none_;
			trả_về true;
		case DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			trả_về true;
		case DriveType::WIRE:
			trả_về wire_.try_append(chunk.wire());
		case DriveType::PORT:
			trả_về port_.try_append(chunk.port());
		case DriveType::MARKER:
			trả_về marker_.try_append(chunk.marker());
		case DriveType::MULTIPLE:
			trả_về multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	nếu (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		trả_về;
	}

	nếu (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	nếu (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	nếu (bits_.empty())
		trả_về;
	std::vector<DriveBit> bits(std::move(bits_));
	cho (auto &bit : bits)
		nếu (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	nếu (chunks_.empty())
		trả_về;
	cho (auto &chunk : chunks_)
	{
		cho (số_nguyên i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	cho (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	nếu (first_edges.emplace(src, dst).first->second == dst)
		trả_về;
	nếu (second_edges.emplace(src, dst).first->second == dst)
		trả_về;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	nếu (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		nếu (found_more->second.empty())
			more_edges.erase(found_more);
		trả_về result;
	}

	auto found_second = second_edges.find(src);
	nếu (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		trả_về result;
	}

	auto found_first = first_edges.find(src);
	nếu (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		trả_về result;
	}

	trả_về DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	trả_về first_edges.count(src);
}

số_nguyên DriverMap::DriveBitGraph::count(DriveBitId src)
{
	nếu (!first_edges.count(src))
		trả_về 0;
	nếu (!second_edges.count(src))
		trả_về 1;
	auto found = more_edges.find(src);
	nếu (found == more_edges.end())
		trả_về 2;
	trả_về GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, số_nguyên index)
{
	nếu (index == 0)
		trả_về first_edges.at(src);
	không_thì nếu (index == 1)
		trả_về second_edges.at(src);
	không_thì
		trả_về *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		case DriveType::NONE:
			trả_về BitMode::NONE;
		case DriveType::CONSTANT:
			// TODO how to handle Sx here?
			trả_về bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		case DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			nếu (driver && !driven)
				trả_về BitMode::DRIVER;
			không_thì nếu (driven && !driver)
				trả_về BitMode::DRIVEN;
			không_thì nếu (driver && driven)
				trả_về BitMode::TRISTATE;
			không_thì
				trả_về keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		case DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			nếu (driver && !driven)
				trả_về BitMode::DRIVER;
			không_thì nếu (driven && !driver)
				trả_về BitMode::DRIVEN_UNIQUE;
			không_thì
				trả_về BitMode::TRISTATE;
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
			trả_về -1;
		case DriveType::CONSTANT:
			trả_về (số_nguyên)bit.constant();
		case DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			số_nguyên offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			nếu (insertion.second) {
				nếu (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} không_thì
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			trả_về insertion.first->second.id + wire_bit.offset;
		}
		case DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			số_nguyên offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			nếu (insertion.second) {
				số_nguyên width = port_bit.cell->connections().at(port_bit.port).size();
				nếu (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} không_thì
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			trả_về insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	nếu (found_isolated != isolated_drive_bits.end())
		trả_về found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	nếu (found == drive_bits.begin()) {
		trả_về id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	nếu (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} không_thì {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	trả_về result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	nếu (driven_id == driver_id)
		trả_về;

	same_driver.merge(driven_id, driver_id);

	cho (số_nguyên i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	cho (số_nguyên i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	cho (auto const &conn : module->connections())
		add(conn.first, conn.second);

	cho (auto cell : module->cells())
		cho (auto const &conn : cell->connections())
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

	nếu (a_id == b_id)
		trả_về;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	nếu (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	không_thì nếu (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	không_thì nếu (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	không_thì nếu (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	không_thì nếu (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	không_thì nếu (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	không_thì
		connect_undirected(a_id, b_id);

	trả_về;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	số_nguyên a_offset = 0;

	auto b_chunk = b_chunks.begin();
	số_nguyên b_offset = 0;

	SigChunk tmp_a, tmp_b;
	trong_khi (a_chunk != a_end) {
		số_nguyên a_width = a_chunk->width - a_offset;
		nếu (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		số_nguyên b_width = b_chunk->width - b_offset;
		nếu (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		số_nguyên width = std::min(a_width, b_width);
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
	số_nguyên offset = 0;
	cho (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, số_nguyên> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	cho (số_nguyên pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		nếu (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		nếu (connected_drivers.contains(current))
			drivers.emplace(current);

		số_nguyên undirected_driver_count = connected_undirected.count(current);

		cho (số_nguyên i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	nếu (drivers.empty())
		cho (auto seen_id : seen)
			drivers.emplace(seen_id);

	cho (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		cho (số_nguyên pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			số_nguyên undirected_driver_count = connected_undirected.count(current);

			cho (số_nguyên i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				nếu (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	cho (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	nếu (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		trả_về bit;
	nếu (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		cho (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		trả_về result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	nếu (mode == BitMode::KEEP && bit_repr_id != bit_id)
		trả_về bit_repr;

	số_nguyên implicit_driver_count = connected_drivers.count(bit_repr_id);
	nếu (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	nếu (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	cho (số_nguyên i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	số_nguyên oriented_driver_count = connected_oriented.count(bit_repr_id);
	cho (số_nguyên i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	trả_về driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	cho (số_nguyên i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	trả_về result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	nếu (chunk.is_whole())
		trả_về id;
	nếu (chunk.width == 1)
		trả_về stringf("%s [%d]", id, chunk.offset);
	trả_về stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	nếu (chunk.is_whole())
		trả_về stringf("%s <%s>", cell_id, port_id);
	nếu (chunk.width == 1)
		trả_về stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	trả_về stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	nếu (chunk.width == 1)
		trả_về stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	trả_về stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		case DriveType::NONE:
			trả_về stringf("<none x%d>", chunk.size());
		case DriveType::CONSTANT:
			trả_về log_const(chunk.constant());
		case DriveType::WIRE:
			trả_về log_signal(chunk.wire());
		case DriveType::PORT:
			trả_về log_signal(chunk.port());
		case DriveType::MARKER:
			trả_về log_signal(chunk.marker());
		case DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const char *sep = " ";
			cho (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			trả_về str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	nếu (chunks.empty())
		trả_về "{}";
	nếu (chunks.size() == 1)
		trả_về log_signal(chunks[0]);

	std::string str;
	const char *sep = "{ ";

	cho (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	trả_về str;
}

YOSYS_NAMESPACE_END
