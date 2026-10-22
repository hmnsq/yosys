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
	ถ้า (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	มิฉะนั้น
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	ถ้า (other.type_ == DriveType::NONE)
		คืน;
	ถ้า (type_ == DriveType::NONE) {
		*this = other;
		คืน;
	}
	ถ้า (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	ถ้า (single.type() == DriveType::NONE)
		คืน;
	ถ้า (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		คืน;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	ถ้า (single.type() == DriveType::NONE)
		คืน;
	ถ้า (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		คืน;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](จำนวนเต็ม i) const
{
	DriveBitMultiple result;
	สำหรับ (auto const &single : multiple_)
		result.merge(single[i]);
	คืน result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	คืน bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	ถ้า (!can_append(bit))
		คืน false;
	width += 1;
	คืน true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	ถ้า (chunk.wire != wire || chunk.offset != offset + width)
		คืน false;
	width += chunk.width;
	คืน true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	คืน bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	ถ้า (!can_append(bit))
		คืน false;
	width += 1;
	คืน true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	ถ้า (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		คืน false;
	width += chunk.width;
	คืน true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	คืน bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	ถ้า (!can_append(bit))
		คืน false;
	width += 1;
	คืน true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	ถ้า (chunk.marker != marker || chunk.offset != offset + width)
		คืน false;
	width += chunk.width;
	คืน true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	ถ้า (bit.multiple().size() != multiple_.size())
		คืน false;

	จำนวนเต็ม const_drivers = 0;
	สำหรับ (DriveChunk const &single : multiple_)
		ถ้า (single.is_constant())
			const_drivers += 1;

	ถ้า (const_drivers > 1)
		คืน false;

	สำหรับ (DriveBit const &single : bit.multiple())
		ถ้า (single.is_constant())
			const_drivers -= 1;

	ถ้า (const_drivers != 0)
		คืน false;

	สำหรับ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				ถ้า (!bit.multiple().count(next))
					คืน false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				ถ้า (!bit.multiple().count(next))
					คืน false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				ถ้า (!bit.multiple().count(next))
					คืน false;
			} bha_nga;
			default:
				คืน false;
		}
	}

	คืน true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	ถ้า (chunk.multiple().size() != multiple_.size())
		คืน false;

	จำนวนเต็ม const_drivers = 0;
	สำหรับ (DriveChunk const &single : multiple_)
		ถ้า (single.is_constant())
			const_drivers += 1;

	ถ้า (const_drivers > 1)
		คืน false;

	สำหรับ (DriveChunk const &single : chunk.multiple())
		ถ้า (single.is_constant())
			const_drivers -= 1;

	ถ้า (const_drivers != 0)
		คืน false;

	สำหรับ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				ถ้า (!chunk.multiple().count(next))
					คืน false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				ถ้า (!chunk.multiple().count(next))
					คืน false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				ถ้า (!chunk.multiple().count(next))
					คืน false;
			} bha_nga;
			default:
				คืน false;
		}
	}

	คืน true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	ถ้า (!can_append(bit))
		คืน false;
	width_ += 1;
	State constant;

	สำหรับ (DriveBit const &single : bit.multiple())
		ถ้า (single.is_constant())
			constant = single.constant();

	สำหรับ (DriveChunk &single : multiple_)
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
	คืน true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	ถ้า (!can_append(chunk))
		คืน false;
	จำนวนเต็ม width = chunk.size();
	width_ += width;
	Const constant;

	สำหรับ (DriveChunk const &single : chunk.multiple())
		ถ้า (single.is_constant())
			constant = single.constant();

	สำหรับ (DriveChunk &single : multiple_)
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
	คืน true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	ถ้า (size() == 0)
		คืน true;
	ถ้า (bit.type() != type_)
		คืน false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			คืน true;
		pak_sa DriveType::CONSTANT:
			คืน true;
		pak_sa DriveType::WIRE:
			คืน wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			คืน port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			คืน multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	ถ้า (size() == 0)
		*this = bit;
	ถ้า (bit.type() != type_)
		คืน false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			คืน true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			คืน true;
		pak_sa DriveType::WIRE:
			คืน wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			คืน port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			คืน multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	ถ้า (size() == 0)
		*this = chunk;
	ถ้า (chunk.type_ != type_)
		คืน false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			คืน true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			คืน true;
		pak_sa DriveType::WIRE:
			คืน wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			คืน port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			คืน marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			คืน multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	ถ้า (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		คืน;
	}

	ถ้า (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	ถ้า (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	ถ้า (bits_.empty())
		คืน;
	std::vector<DriveBit> bits(std::move(bits_));
	สำหรับ (auto &bit : bits)
		ถ้า (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	ถ้า (chunks_.empty())
		คืน;
	สำหรับ (auto &chunk : chunks_)
	{
		สำหรับ (จำนวนเต็ม i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	สำหรับ (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	ถ้า (first_edges.emplace(src, dst).first->second == dst)
		คืน;
	ถ้า (second_edges.emplace(src, dst).first->second == dst)
		คืน;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	ถ้า (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		ถ้า (found_more->second.empty())
			more_edges.erase(found_more);
		คืน result;
	}

	auto found_second = second_edges.find(src);
	ถ้า (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		คืน result;
	}

	auto found_first = first_edges.find(src);
	ถ้า (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		คืน result;
	}

	คืน DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	คืน first_edges.count(src);
}

จำนวนเต็ม DriverMap::DriveBitGraph::count(DriveBitId src)
{
	ถ้า (!first_edges.count(src))
		คืน 0;
	ถ้า (!second_edges.count(src))
		คืน 1;
	auto found = more_edges.find(src);
	ถ้า (found == more_edges.end())
		คืน 2;
	คืน GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, จำนวนเต็ม index)
{
	ถ้า (index == 0)
		คืน first_edges.at(src);
	มิฉะนั้น ถ้า (index == 1)
		คืน second_edges.at(src);
	มิฉะนั้น
		คืน *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			คืน BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			คืน bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			ถ้า (driver && !driven)
				คืน BitMode::DRIVER;
			มิฉะนั้น ถ้า (driven && !driver)
				คืน BitMode::DRIVEN;
			มิฉะนั้น ถ้า (driver && driven)
				คืน BitMode::TRISTATE;
			มิฉะนั้น
				คืน keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			ถ้า (driver && !driven)
				คืน BitMode::DRIVER;
			มิฉะนั้น ถ้า (driven && !driver)
				คืน BitMode::DRIVEN_UNIQUE;
			มิฉะนั้น
				คืน BitMode::TRISTATE;
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
			คืน -1;
		pak_sa DriveType::CONSTANT:
			คืน (จำนวนเต็ม)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			จำนวนเต็ม offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			ถ้า (insertion.second) {
				ถ้า (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} มิฉะนั้น
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			คืน insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			จำนวนเต็ม offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			ถ้า (insertion.second) {
				จำนวนเต็ม width = port_bit.cell->connections().at(port_bit.port).size();
				ถ้า (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} มิฉะนั้น
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			คืน insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	ถ้า (found_isolated != isolated_drive_bits.end())
		คืน found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	ถ้า (found == drive_bits.begin()) {
		คืน id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	ถ้า (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} มิฉะนั้น {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	คืน result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	ถ้า (driven_id == driver_id)
		คืน;

	same_driver.merge(driven_id, driver_id);

	สำหรับ (จำนวนเต็ม i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	สำหรับ (จำนวนเต็ม i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	สำหรับ (auto const &conn : module->connections())
		add(conn.first, conn.second);

	สำหรับ (auto cell : module->cells())
		สำหรับ (auto const &conn : cell->connections())
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

	ถ้า (a_id == b_id)
		คืน;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ถ้า (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	มิฉะนั้น ถ้า (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	มิฉะนั้น ถ้า (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	มิฉะนั้น ถ้า (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	มิฉะนั้น ถ้า (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	มิฉะนั้น ถ้า (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	มิฉะนั้น
		connect_undirected(a_id, b_id);

	คืน;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	จำนวนเต็ม a_offset = 0;

	auto b_chunk = b_chunks.begin();
	จำนวนเต็ม b_offset = 0;

	SigChunk tmp_a, tmp_b;
	ขณะ (a_chunk != a_end) {
		จำนวนเต็ม a_width = a_chunk->width - a_offset;
		ถ้า (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		จำนวนเต็ม b_width = b_chunk->width - b_offset;
		ถ้า (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		จำนวนเต็ม width = std::min(a_width, b_width);
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
	จำนวนเต็ม offset = 0;
	สำหรับ (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, จำนวนเต็ม> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	สำหรับ (จำนวนเต็ม pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		ถ้า (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		ถ้า (connected_drivers.contains(current))
			drivers.emplace(current);

		จำนวนเต็ม undirected_driver_count = connected_undirected.count(current);

		สำหรับ (จำนวนเต็ม i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	ถ้า (drivers.empty())
		สำหรับ (auto seen_id : seen)
			drivers.emplace(seen_id);

	สำหรับ (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		สำหรับ (จำนวนเต็ม pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			จำนวนเต็ม undirected_driver_count = connected_undirected.count(current);

			สำหรับ (จำนวนเต็ม i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				ถ้า (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	สำหรับ (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	ถ้า (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		คืน bit;
	ถ้า (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		สำหรับ (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		คืน result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	ถ้า (mode == BitMode::KEEP && bit_repr_id != bit_id)
		คืน bit_repr;

	จำนวนเต็ม implicit_driver_count = connected_drivers.count(bit_repr_id);
	ถ้า (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	ถ้า (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	สำหรับ (จำนวนเต็ม i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	จำนวนเต็ม oriented_driver_count = connected_oriented.count(bit_repr_id);
	สำหรับ (จำนวนเต็ม i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	คืน driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	สำหรับ (จำนวนเต็ม i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	คืน result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ถ้า (chunk.is_whole())
		คืน id;
	ถ้า (chunk.width == 1)
		คืน stringf("%s [%d]", id, chunk.offset);
	คืน stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	ถ้า (chunk.is_whole())
		คืน stringf("%s <%s>", cell_id, port_id);
	ถ้า (chunk.width == 1)
		คืน stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	คืน stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	ถ้า (chunk.width == 1)
		คืน stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	คืน stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			คืน stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			คืน log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			คืน log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			คืน log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			คืน log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			สำหรับ (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			คืน str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	ถ้า (chunks.empty())
		คืน "{}";
	ถ้า (chunks.size() == 1)
		คืน log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	สำหรับ (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	คืน str;
}

YOSYS_NAMESPACE_END
