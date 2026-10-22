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
	නම් (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	නැතහොත්
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	නම් (other.type_ == DriveType::NONE)
		ආපසු;
	නම් (type_ == DriveType::NONE) {
		*this = other;
		ආපසු;
	}
	නම් (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	නම් (single.type() == DriveType::NONE)
		ආපසු;
	නම් (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ආපසු;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	නම් (single.type() == DriveType::NONE)
		ආපසු;
	නම් (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ආපසු;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](පූර්ණ_සංඛ්‍යා i) const
{
	DriveBitMultiple result;
	සඳහා (auto const &single : multiple_)
		result.merge(single[i]);
	ආපසු result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	ආපසු bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	නම් (!can_append(bit))
		ආපසු false;
	width += 1;
	ආපසු true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	නම් (chunk.wire != wire || chunk.offset != offset + width)
		ආපසු false;
	width += chunk.width;
	ආපසු true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	ආපසු bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	නම් (!can_append(bit))
		ආපසු false;
	width += 1;
	ආපසු true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	නම් (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ආපසු false;
	width += chunk.width;
	ආපසු true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	ආපසු bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	නම් (!can_append(bit))
		ආපසු false;
	width += 1;
	ආපසු true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	නම් (chunk.marker != marker || chunk.offset != offset + width)
		ආපසු false;
	width += chunk.width;
	ආපසු true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	නම් (bit.multiple().size() != multiple_.size())
		ආපසු false;

	පූර්ණ_සංඛ්‍යා const_drivers = 0;
	සඳහා (DriveChunk const &single : multiple_)
		නම් (single.is_constant())
			const_drivers += 1;

	නම් (const_drivers > 1)
		ආපසු false;

	සඳහා (DriveBit const &single : bit.multiple())
		නම් (single.is_constant())
			const_drivers -= 1;

	නම් (const_drivers != 0)
		ආපසු false;

	සඳහා (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				නම් (!bit.multiple().count(next))
					ආපසු false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				නම් (!bit.multiple().count(next))
					ආපසු false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				නම් (!bit.multiple().count(next))
					ආපසු false;
			} bha_nga;
			default:
				ආපසු false;
		}
	}

	ආපසු true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	නම් (chunk.multiple().size() != multiple_.size())
		ආපසු false;

	පූර්ණ_සංඛ්‍යා const_drivers = 0;
	සඳහා (DriveChunk const &single : multiple_)
		නම් (single.is_constant())
			const_drivers += 1;

	නම් (const_drivers > 1)
		ආපසු false;

	සඳහා (DriveChunk const &single : chunk.multiple())
		නම් (single.is_constant())
			const_drivers -= 1;

	නම් (const_drivers != 0)
		ආපසු false;

	සඳහා (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				නම් (!chunk.multiple().count(next))
					ආපසු false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				නම් (!chunk.multiple().count(next))
					ආපසු false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				නම් (!chunk.multiple().count(next))
					ආපසු false;
			} bha_nga;
			default:
				ආපසු false;
		}
	}

	ආපසු true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	නම් (!can_append(bit))
		ආපසු false;
	width_ += 1;
	State constant;

	සඳහා (DriveBit const &single : bit.multiple())
		නම් (single.is_constant())
			constant = single.constant();

	සඳහා (DriveChunk &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} bha_nga;
			pak_sa DriveType::WIRE: {
				single.wire().width += 1;
			} bha_nga;
			pak_sa DriveType::PORT: {
				single.port().width += 1;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				single.marker().width += 1;
			} bha_nga;
			default:
				log_abort();
		}
	}
	ආපසු true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	නම් (!can_append(chunk))
		ආපසු false;
	පූර්ණ_සංඛ්‍යා width = chunk.size();
	width_ += width;
	Const constant;

	සඳහා (DriveChunk const &single : chunk.multiple())
		නම් (single.is_constant())
			constant = single.constant();

	සඳහා (DriveChunk &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
				single.constant().append(constant);
			} bha_nga;
			pak_sa DriveType::WIRE: {
				single.wire().width += width;
			} bha_nga;
			pak_sa DriveType::PORT: {
				single.port().width += width;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				single.marker().width += width;
			} bha_nga;
			default:
				log_abort();
		}
	}
	ආපසු true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	නම් (size() == 0)
		ආපසු true;
	නම් (bit.type() != type_)
		ආපසු false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			ආපසු true;
		pak_sa DriveType::CONSTANT:
			ආපසු true;
		pak_sa DriveType::WIRE:
			ආපසු wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			ආපසු port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ආපසු multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	නම් (size() == 0)
		*this = bit;
	නම් (bit.type() != type_)
		ආපසු false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			ආපසු true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ආපසු true;
		pak_sa DriveType::WIRE:
			ආපසු wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			ආපසු port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ආපසු multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	නම් (size() == 0)
		*this = chunk;
	නම් (chunk.type_ != type_)
		ආපසු false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			ආපසු true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ආපසු true;
		pak_sa DriveType::WIRE:
			ආපසු wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			ආපසු port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			ආපසු marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			ආපසු multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	නම් (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		ආපසු;
	}

	නම් (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	නම් (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	නම් (bits_.empty())
		ආපසු;
	std::vector<DriveBit> bits(std::move(bits_));
	සඳහා (auto &bit : bits)
		නම් (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	නම් (chunks_.empty())
		ආපසු;
	සඳහා (auto &chunk : chunks_)
	{
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	සඳහා (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	නම් (first_edges.emplace(src, dst).first->second == dst)
		ආපසු;
	නම් (second_edges.emplace(src, dst).first->second == dst)
		ආපසු;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	නම් (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		නම් (found_more->second.empty())
			more_edges.erase(found_more);
		ආපසු result;
	}

	auto found_second = second_edges.find(src);
	නම් (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ආපසු result;
	}

	auto found_first = first_edges.find(src);
	නම් (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ආපසු result;
	}

	ආපසු DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ආපසු first_edges.count(src);
}

පූර්ණ_සංඛ්‍යා DriverMap::DriveBitGraph::count(DriveBitId src)
{
	නම් (!first_edges.count(src))
		ආපසු 0;
	නම් (!second_edges.count(src))
		ආපසු 1;
	auto found = more_edges.find(src);
	නම් (found == more_edges.end())
		ආපසු 2;
	ආපසු GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, පූර්ණ_සංඛ්‍යා index)
{
	නම් (index == 0)
		ආපසු first_edges.at(src);
	නැතහොත් නම් (index == 1)
		ආපසු second_edges.at(src);
	නැතහොත්
		ආපසු *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ආපසු BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ආපසු bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			නම් (driver && !driven)
				ආපසු BitMode::DRIVER;
			නැතහොත් නම් (driven && !driver)
				ආපසු BitMode::DRIVEN;
			නැතහොත් නම් (driver && driven)
				ආපසු BitMode::TRISTATE;
			නැතහොත්
				ආපසු keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			නම් (driver && !driven)
				ආපසු BitMode::DRIVER;
			නැතහොත් නම් (driven && !driver)
				ආපසු BitMode::DRIVEN_UNIQUE;
			නැතහොත්
				ආපසු BitMode::TRISTATE;
		}
		pak_sa DriveType::MARKER: {
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
		pak_sa DriveType::NONE:
			ආපසු -1;
		pak_sa DriveType::CONSTANT:
			ආපසු (පූර්ණ_සංඛ්‍යා)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			පූර්ණ_සංඛ්‍යා offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			නම් (insertion.second) {
				නම් (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} නැතහොත්
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ආපසු insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			පූර්ණ_සංඛ්‍යා offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			නම් (insertion.second) {
				පූර්ණ_සංඛ්‍යා width = port_bit.cell->connections().at(port_bit.port).size();
				නම් (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} නැතහොත්
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ආපසු insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	නම් (found_isolated != isolated_drive_bits.end())
		ආපසු found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	නම් (found == drive_bits.begin()) {
		ආපසු id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	නම් (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} නැතහොත් {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ආපසු result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	නම් (driven_id == driver_id)
		ආපසු;

	same_driver.merge(driven_id, driver_id);

	සඳහා (පූර්ණ_සංඛ්‍යා i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	සඳහා (පූර්ණ_සංඛ්‍යා i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	සඳහා (auto const &conn : module->connections())
		add(conn.first, conn.second);

	සඳහා (auto cell : module->cells())
		සඳහා (auto const &conn : cell->connections())
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

	නම් (a_id == b_id)
		ආපසු;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	නම් (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	නැතහොත් නම් (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	නැතහොත් නම් (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	නැතහොත් නම් (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	නැතහොත් නම් (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	නැතහොත් නම් (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	නැතහොත්
		connect_undirected(a_id, b_id);

	ආපසු;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	පූර්ණ_සංඛ්‍යා a_offset = 0;

	auto b_chunk = b_chunks.begin();
	පූර්ණ_සංඛ්‍යා b_offset = 0;

	SigChunk tmp_a, tmp_b;
	අතරතුර (a_chunk != a_end) {
		පූර්ණ_සංඛ්‍යා a_width = a_chunk->width - a_offset;
		නම් (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		පූර්ණ_සංඛ්‍යා b_width = b_chunk->width - b_offset;
		නම් (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		පූර්ණ_සංඛ්‍යා width = std::min(a_width, b_width);
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
	පූර්ණ_සංඛ්‍යා offset = 0;
	සඳහා (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, පූර්ණ_සංඛ්‍යා> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	සඳහා (පූර්ණ_සංඛ්‍යා pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		නම් (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		නම් (connected_drivers.contains(current))
			drivers.emplace(current);

		පූර්ණ_සංඛ්‍යා undirected_driver_count = connected_undirected.count(current);

		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	නම් (drivers.empty())
		සඳහා (auto seen_id : seen)
			drivers.emplace(seen_id);

	සඳහා (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		සඳහා (පූර්ණ_සංඛ්‍යා pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			පූර්ණ_සංඛ්‍යා undirected_driver_count = connected_undirected.count(current);

			සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				නම් (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	සඳහා (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	නම් (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ආපසු bit;
	නම් (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		සඳහා (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		ආපසු result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	නම් (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ආපසු bit_repr;

	පූර්ණ_සංඛ්‍යා implicit_driver_count = connected_drivers.count(bit_repr_id);
	නම් (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	නම් (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	පූර්ණ_සංඛ්‍යා oriented_driver_count = connected_oriented.count(bit_repr_id);
	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ආපසු driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	සඳහා (පූර්ණ_සංඛ්‍යා i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	ආපසු result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	නම් (chunk.is_whole())
		ආපසු id;
	නම් (chunk.width == 1)
		ආපසු stringf("%s [%d]", id, chunk.offset);
	ආපසු stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	නම් (chunk.is_whole())
		ආපසු stringf("%s <%s>", cell_id, port_id);
	නම් (chunk.width == 1)
		ආපසු stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ආපසු stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	නම් (chunk.width == 1)
		ආපසු stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ආපසු stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			ආපසු stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			ආපසු log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			ආපසු log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			ආපසු log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			ආපසු log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			සඳහා (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ආපසු str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	නම් (chunks.empty())
		ආපසු "{}";
	නම් (chunks.size() == 1)
		ආපසු log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	සඳහා (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ආපසු str;
}

YOSYS_NAMESPACE_END
