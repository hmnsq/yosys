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
	ако (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	инако
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	ако (other.type_ == DriveType::NONE)
		врати;
	ако (type_ == DriveType::NONE) {
		*this = other;
		врати;
	}
	ако (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	ако (single.type() == DriveType::NONE)
		врати;
	ако (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		врати;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	ако (single.type() == DriveType::NONE)
		врати;
	ако (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		врати;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](цео i) const
{
	DriveBitMultiple result;
	за (auto const &single : multiple_)
		result.merge(single[i]);
	врати result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	врати bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	ако (!can_append(bit))
		врати false;
	width += 1;
	врати true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	ако (chunk.wire != wire || chunk.offset != offset + width)
		врати false;
	width += chunk.width;
	врати true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	врати bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	ако (!can_append(bit))
		врати false;
	width += 1;
	врати true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	ако (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		врати false;
	width += chunk.width;
	врати true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	врати bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	ако (!can_append(bit))
		врати false;
	width += 1;
	врати true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	ако (chunk.marker != marker || chunk.offset != offset + width)
		врати false;
	width += chunk.width;
	врати true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	ако (bit.multiple().size() != multiple_.size())
		врати false;

	цео const_drivers = 0;
	за (DriveChunk const &single : multiple_)
		ако (single.is_constant())
			const_drivers += 1;

	ако (const_drivers > 1)
		врати false;

	за (DriveBit const &single : bit.multiple())
		ако (single.is_constant())
			const_drivers -= 1;

	ако (const_drivers != 0)
		врати false;

	за (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
			} break;
			case DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				ако (!bit.multiple().count(next))
					врати false;
			} break;
			case DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				ако (!bit.multiple().count(next))
					врати false;
			} break;
			case DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				ако (!bit.multiple().count(next))
					врати false;
			} break;
			default:
				врати false;
		}
	}

	врати true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	ако (chunk.multiple().size() != multiple_.size())
		врати false;

	цео const_drivers = 0;
	за (DriveChunk const &single : multiple_)
		ако (single.is_constant())
			const_drivers += 1;

	ако (const_drivers > 1)
		врати false;

	за (DriveChunk const &single : chunk.multiple())
		ако (single.is_constant())
			const_drivers -= 1;

	ако (const_drivers != 0)
		врати false;

	за (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			case DriveType::CONSTANT: {
			} break;
			case DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				ако (!chunk.multiple().count(next))
					врати false;
			} break;
			case DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				ако (!chunk.multiple().count(next))
					врати false;
			} break;
			case DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				ако (!chunk.multiple().count(next))
					врати false;
			} break;
			default:
				врати false;
		}
	}

	врати true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	ако (!can_append(bit))
		врати false;
	width_ += 1;
	State constant;

	за (DriveBit const &single : bit.multiple())
		ако (single.is_constant())
			constant = single.constant();

	за (DriveChunk &single : multiple_)
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
	врати true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	ако (!can_append(chunk))
		врати false;
	цео width = chunk.size();
	width_ += width;
	Const constant;

	за (DriveChunk const &single : chunk.multiple())
		ако (single.is_constant())
			constant = single.constant();

	за (DriveChunk &single : multiple_)
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
	врати true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	ако (size() == 0)
		врати true;
	ако (bit.type() != type_)
		врати false;
	switch (type_)
	{
		case DriveType::NONE:
			врати true;
		case DriveType::CONSTANT:
			врати true;
		case DriveType::WIRE:
			врати wire_.can_append(bit.wire());
		case DriveType::PORT:
			врати port_.can_append(bit.port());
		case DriveType::MULTIPLE:
			врати multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	ако (size() == 0)
		*this = bit;
	ако (bit.type() != type_)
		врати false;
	switch (type_)
	{
		case DriveType::NONE:
			none_ += 1;
			врати true;
		case DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			врати true;
		case DriveType::WIRE:
			врати wire_.try_append(bit.wire());
		case DriveType::PORT:
			врати port_.try_append(bit.port());
		case DriveType::MULTIPLE:
			врати multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	ако (size() == 0)
		*this = chunk;
	ако (chunk.type_ != type_)
		врати false;
	switch (type_)
	{
		case DriveType::NONE:
			none_ += chunk.none_;
			врати true;
		case DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			врати true;
		case DriveType::WIRE:
			врати wire_.try_append(chunk.wire());
		case DriveType::PORT:
			врати port_.try_append(chunk.port());
		case DriveType::MARKER:
			врати marker_.try_append(chunk.marker());
		case DriveType::MULTIPLE:
			врати multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	ако (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		врати;
	}

	ако (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	ако (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	ако (bits_.empty())
		врати;
	std::vector<DriveBit> bits(std::move(bits_));
	за (auto &bit : bits)
		ако (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	ако (chunks_.empty())
		врати;
	за (auto &chunk : chunks_)
	{
		за (цео i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	за (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	ако (first_edges.emplace(src, dst).first->second == dst)
		врати;
	ако (second_edges.emplace(src, dst).first->second == dst)
		врати;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	ако (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		ако (found_more->second.empty())
			more_edges.erase(found_more);
		врати result;
	}

	auto found_second = second_edges.find(src);
	ако (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		врати result;
	}

	auto found_first = first_edges.find(src);
	ако (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		врати result;
	}

	врати DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	врати first_edges.count(src);
}

цео DriverMap::DriveBitGraph::count(DriveBitId src)
{
	ако (!first_edges.count(src))
		врати 0;
	ако (!second_edges.count(src))
		врати 1;
	auto found = more_edges.find(src);
	ако (found == more_edges.end())
		врати 2;
	врати GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, цео index)
{
	ако (index == 0)
		врати first_edges.at(src);
	инако ако (index == 1)
		врати second_edges.at(src);
	инако
		врати *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		case DriveType::NONE:
			врати BitMode::NONE;
		case DriveType::CONSTANT:
			// TODO how to handle Sx here?
			врати bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		case DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			ако (driver && !driven)
				врати BitMode::DRIVER;
			инако ако (driven && !driver)
				врати BitMode::DRIVEN;
			инако ако (driver && driven)
				врати BitMode::TRISTATE;
			инако
				врати keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		case DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			ако (driver && !driven)
				врати BitMode::DRIVER;
			инако ако (driven && !driver)
				врати BitMode::DRIVEN_UNIQUE;
			инако
				врати BitMode::TRISTATE;
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
			врати -1;
		case DriveType::CONSTANT:
			врати (цео)bit.constant();
		case DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			цео offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			ако (insertion.second) {
				ако (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} инако
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			врати insertion.first->second.id + wire_bit.offset;
		}
		case DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			цео offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			ако (insertion.second) {
				цео width = port_bit.cell->connections().at(port_bit.port).size();
				ако (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} инако
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			врати insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	ако (found_isolated != isolated_drive_bits.end())
		врати found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	ако (found == drive_bits.begin()) {
		врати id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	ако (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} инако {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	врати result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	ако (driven_id == driver_id)
		врати;

	same_driver.merge(driven_id, driver_id);

	за (цео i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	за (цео i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	за (auto const &conn : module->connections())
		add(conn.first, conn.second);

	за (auto cell : module->cells())
		за (auto const &conn : cell->connections())
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

	ако (a_id == b_id)
		врати;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ако (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	инако ако (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	инако ако (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	инако ако (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	инако ако (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	инако ако (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	инако
		connect_undirected(a_id, b_id);

	врати;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	цео a_offset = 0;

	auto b_chunk = b_chunks.begin();
	цео b_offset = 0;

	SigChunk tmp_a, tmp_b;
	док (a_chunk != a_end) {
		цео a_width = a_chunk->width - a_offset;
		ако (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		цео b_width = b_chunk->width - b_offset;
		ако (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		цео width = std::min(a_width, b_width);
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
	цео offset = 0;
	за (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, цео> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	за (цео pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		ако (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		ако (connected_drivers.contains(current))
			drivers.emplace(current);

		цео undirected_driver_count = connected_undirected.count(current);

		за (цео i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	ако (drivers.empty())
		за (auto seen_id : seen)
			drivers.emplace(seen_id);

	за (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		за (цео pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			цео undirected_driver_count = connected_undirected.count(current);

			за (цео i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				ако (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	за (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	ако (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		врати bit;
	ако (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		за (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		врати result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	ако (mode == BitMode::KEEP && bit_repr_id != bit_id)
		врати bit_repr;

	цео implicit_driver_count = connected_drivers.count(bit_repr_id);
	ако (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	ако (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	за (цео i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	цео oriented_driver_count = connected_oriented.count(bit_repr_id);
	за (цео i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	врати driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	за (цео i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	врати result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ако (chunk.is_whole())
		врати id;
	ако (chunk.width == 1)
		врати stringf("%s [%d]", id, chunk.offset);
	врати stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	ако (chunk.is_whole())
		врати stringf("%s <%s>", cell_id, port_id);
	ако (chunk.width == 1)
		врати stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	врати stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	ако (chunk.width == 1)
		врати stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	врати stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		case DriveType::NONE:
			врати stringf("<none x%d>", chunk.size());
		case DriveType::CONSTANT:
			врати log_const(chunk.constant());
		case DriveType::WIRE:
			врати log_signal(chunk.wire());
		case DriveType::PORT:
			врати log_signal(chunk.port());
		case DriveType::MARKER:
			врати log_signal(chunk.marker());
		case DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const char *sep = " ";
			за (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			врати str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	ако (chunks.empty())
		врати "{}";
	ако (chunks.size() == 1)
		врати log_signal(chunks[0]);

	std::string str;
	const char *sep = "{ ";

	за (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	врати str;
}

YOSYS_NAMESPACE_END
