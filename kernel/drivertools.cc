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

#અન્તર્ભાવ "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit અચલ &bit)
{
	જો (bit.is_wire())
		*આ = DriveBitWire(bit.wire, bit.offset);
	નહીંતર
		*આ = bit.data;
}

શૂન્ય DriveBit::merge(DriveBit અચલ &other)
{
	જો (other.type_ == DriveType::NONE)
		પાછા;
	જો (type_ == DriveType::NONE) {
		*આ = other;
		પાછા;
	}
	જો (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*આ));
		*આ = std::move(multi);
	}
	multiple().merge(other);
}


શૂન્ય DriveBitMultiple::merge(DriveBit અચલ &single)
{
	જો (single.type() == DriveType::NONE)
		પાછા;
	જો (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		પાછા;
	}
	multiple_.emplace(single);
}

શૂન્ય DriveBitMultiple::merge(DriveBit &&single)
{
	જો (single.type() == DriveType::NONE)
		પાછા;
	જો (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		પાછા;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](પૂર્ણાંક i) અચલ
{
	DriveBitMultiple result;
	માટે (auto અચલ &single : multiple_)
		result.merge(single[i]);
	પાછા result;
}


તાર્કિક DriveChunkWire::can_append(DriveBitWire અચલ &bit) અચલ
{
	પાછા bit.wire == wire && bit.offset == offset + width;
}

તાર્કિક DriveChunkWire::try_append(DriveBitWire અચલ &bit)
{
	જો (!can_append(bit))
		પાછા ખોટું;
	width += 1;
	પાછા સાચું;
}

તાર્કિક DriveChunkWire::try_append(DriveChunkWire અચલ &chunk)
{
	જો (chunk.wire != wire || chunk.offset != offset + width)
		પાછા ખોટું;
	width += chunk.width;
	પાછા સાચું;
}

તાર્કિક DriveChunkPort::can_append(DriveBitPort અચલ &bit) અચલ
{
	પાછા bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

તાર્કિક DriveChunkPort::try_append(DriveBitPort અચલ &bit)
{
	જો (!can_append(bit))
		પાછા ખોટું;
	width += 1;
	પાછા સાચું;
}

તાર્કિક DriveChunkPort::try_append(DriveChunkPort અચલ &chunk)
{
	જો (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		પાછા ખોટું;
	width += chunk.width;
	પાછા સાચું;
}

તાર્કિક DriveChunkMarker::can_append(DriveBitMarker અચલ &bit) અચલ
{
	પાછા bit.marker == marker && bit.offset == offset + width;
}

તાર્કિક DriveChunkMarker::try_append(DriveBitMarker અચલ &bit)
{
	જો (!can_append(bit))
		પાછા ખોટું;
	width += 1;
	પાછા સાચું;
}

તાર્કિક DriveChunkMarker::try_append(DriveChunkMarker અચલ &chunk)
{
	જો (chunk.marker != marker || chunk.offset != offset + width)
		પાછા ખોટું;
	width += chunk.width;
	પાછા સાચું;
}


તાર્કિક DriveChunkMultiple::can_append(DriveBitMultiple અચલ &bit) અચલ
{
	જો (bit.multiple().size() != multiple_.size())
		પાછા ખોટું;

	પૂર્ણાંક const_drivers = 0;
	માટે (DriveChunk અચલ &single : multiple_)
		જો (single.is_constant())
			const_drivers += 1;

	જો (const_drivers > 1)
		પાછા ખોટું;

	માટે (DriveBit અચલ &single : bit.multiple())
		જો (single.is_constant())
			const_drivers -= 1;

	જો (const_drivers != 0)
		પાછા ખોટું;

	માટે (DriveChunk અચલ &single : multiple_)
	{
		પસંદગી (single.type())
		{
			સ્થિતિ DriveType::CONSTANT: {
			} તોડો;
			સ્થિતિ DriveType::WIRE: {
				auto અચલ &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				જો (!bit.multiple().ગણતરી(next))
					પાછા ખોટું;
			} તોડો;
			સ્થિતિ DriveType::PORT: {
				auto અચલ &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				જો (!bit.multiple().ગણતરી(next))
					પાછા ખોટું;
			} તોડો;
			સ્થિતિ DriveType::MARKER: {
				auto અચલ &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				જો (!bit.multiple().ગણતરી(next))
					પાછા ખોટું;
			} તોડો;
			મૂળભૂત:
				પાછા ખોટું;
		}
	}

	પાછા સાચું;
}

તાર્કિક DriveChunkMultiple::can_append(DriveChunkMultiple અચલ &chunk) અચલ
{
	જો (chunk.multiple().size() != multiple_.size())
		પાછા ખોટું;

	પૂર્ણાંક const_drivers = 0;
	માટે (DriveChunk અચલ &single : multiple_)
		જો (single.is_constant())
			const_drivers += 1;

	જો (const_drivers > 1)
		પાછા ખોટું;

	માટે (DriveChunk અચલ &single : chunk.multiple())
		જો (single.is_constant())
			const_drivers -= 1;

	જો (const_drivers != 0)
		પાછા ખોટું;

	માટે (DriveChunk અચલ &single : multiple_)
	{
		પસંદગી (single.type())
		{
			સ્થિતિ DriveType::CONSTANT: {
			} તોડો;
			સ્થિતિ DriveType::WIRE: {
				auto અચલ &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				જો (!chunk.multiple().ગણતરી(next))
					પાછા ખોટું;
			} તોડો;
			સ્થિતિ DriveType::PORT: {
				auto અચલ &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				જો (!chunk.multiple().ગણતરી(next))
					પાછા ખોટું;
			} તોડો;
			સ્થિતિ DriveType::MARKER: {
				auto અચલ &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				જો (!chunk.multiple().ગણતરી(next))
					પાછા ખોટું;
			} તોડો;
			મૂળભૂત:
				પાછા ખોટું;
		}
	}

	પાછા સાચું;
}

તાર્કિક DriveChunkMultiple::try_append(DriveBitMultiple અચલ &bit)
{
	જો (!can_append(bit))
		પાછા ખોટું;
	width_ += 1;
	State constant;

	માટે (DriveBit અચલ &single : bit.multiple())
		જો (single.is_constant())
			constant = single.constant();

	માટે (DriveChunk &single : multiple_)
	{
		પસંદગી (single.type())
		{
			સ્થિતિ DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} તોડો;
			સ્થિતિ DriveType::WIRE: {
				single.wire().width += 1;
			} તોડો;
			સ્થિતિ DriveType::PORT: {
				single.port().width += 1;
			} તોડો;
			સ્થિતિ DriveType::MARKER: {
				single.marker().width += 1;
			} તોડો;
			મૂળભૂત:
				log_abort();
		}
	}
	પાછા સાચું;
}

તાર્કિક DriveChunkMultiple::try_append(DriveChunkMultiple અચલ &chunk)
{
	જો (!can_append(chunk))
		પાછા ખોટું;
	પૂર્ણાંક width = chunk.size();
	width_ += width;
	Const constant;

	માટે (DriveChunk અચલ &single : chunk.multiple())
		જો (single.is_constant())
			constant = single.constant();

	માટે (DriveChunk &single : multiple_)
	{
		પસંદગી (single.type())
		{
			સ્થિતિ DriveType::CONSTANT: {
				single.constant().append(constant);
			} તોડો;
			સ્થિતિ DriveType::WIRE: {
				single.wire().width += width;
			} તોડો;
			સ્થિતિ DriveType::PORT: {
				single.port().width += width;
			} તોડો;
			સ્થિતિ DriveType::MARKER: {
				single.marker().width += width;
			} તોડો;
			મૂળભૂત:
				log_abort();
		}
	}
	પાછા સાચું;
}

તાર્કિક DriveChunk::can_append(DriveBit અચલ &bit) અચલ
{
	જો (size() == 0)
		પાછા સાચું;
	જો (bit.type() != type_)
		પાછા ખોટું;
	પસંદગી (type_)
	{
		સ્થિતિ DriveType::NONE:
			પાછા સાચું;
		સ્થિતિ DriveType::CONSTANT:
			પાછા સાચું;
		સ્થિતિ DriveType::WIRE:
			પાછા wire_.can_append(bit.wire());
		સ્થિતિ DriveType::PORT:
			પાછા port_.can_append(bit.port());
		સ્થિતિ DriveType::MULTIPLE:
			પાછા multiple_.can_append(bit.multiple());
		મૂળભૂત:
			log_abort();
	}
}

તાર્કિક DriveChunk::try_append(DriveBit અચલ &bit)
{
	જો (size() == 0)
		*આ = bit;
	જો (bit.type() != type_)
		પાછા ખોટું;
	પસંદગી (type_)
	{
		સ્થિતિ DriveType::NONE:
			none_ += 1;
			પાછા સાચું;
		સ્થિતિ DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			પાછા સાચું;
		સ્થિતિ DriveType::WIRE:
			પાછા wire_.try_append(bit.wire());
		સ્થિતિ DriveType::PORT:
			પાછા port_.try_append(bit.port());
		સ્થિતિ DriveType::MULTIPLE:
			પાછા multiple_.try_append(bit.multiple());
		મૂળભૂત:
			log_abort();
	}
}


તાર્કિક DriveChunk::try_append(DriveChunk અચલ &chunk)
{
	જો (size() == 0)
		*આ = chunk;
	જો (chunk.type_ != type_)
		પાછા ખોટું;
	પસંદગી (type_)
	{
		સ્થિતિ DriveType::NONE:
			none_ += chunk.none_;
			પાછા સાચું;
		સ્થિતિ DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			પાછા સાચું;
		સ્થિતિ DriveType::WIRE:
			પાછા wire_.try_append(chunk.wire());
		સ્થિતિ DriveType::PORT:
			પાછા port_.try_append(chunk.port());
		સ્થિતિ DriveType::MARKER:
			પાછા marker_.try_append(chunk.marker());
		સ્થિતિ DriveType::MULTIPLE:
			પાછા multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

શૂન્ય DriveSpec::append(DriveBit અચલ &bit)
{
	hash_ = 0;
	જો (!packed()) {
		bits_.ધકેલો_back(bit);
		width_ += 1;
		પાછા;
	}

	જો (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

શૂન્ય DriveSpec::append(DriveChunk અચલ &chunk)
{
	hash_ = 0;
	pack();
	જો (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

શૂન્ય DriveSpec::pack() અચલ {
	જો (bits_.empty())
		પાછા;
	std::vector<DriveBit> bits(std::move(bits_));
	માટે (auto &bit : bits)
		જો (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

શૂન્ય DriveSpec::unpack() અચલ {
	જો (chunks_.empty())
		પાછા;
	માટે (auto &chunk : chunks_)
	{
		માટે (પૂર્ણાંક i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

શૂન્ય DriveSpec::compute_width()
{
	width_ = 0;
	માટે (auto અચલ &chunk : chunks_)
		width_ += chunk.size();
}

શૂન્ય DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	જો (first_edges.emplace(src, dst).first->second == dst)
		પાછા;
	જો (second_edges.emplace(src, dst).first->second == dst)
		પાછા;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::કાઢો_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	જો (found_more != more_edges.end()) {
		auto result = found_more->second.કાઢો();
		જો (found_more->second.empty())
			more_edges.erase(found_more);
		પાછા result;
	}

	auto found_second = second_edges.find(src);
	જો (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		પાછા result;
	}

	auto found_first = first_edges.find(src);
	જો (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		પાછા result;
	}

	પાછા DriveBitId();
}

શૂન્ય DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

તાર્કિક DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	પાછા first_edges.ગણતરી(src);
}

પૂર્ણાંક DriverMap::DriveBitGraph::ગણતરી(DriveBitId src)
{
	જો (!first_edges.ગણતરી(src))
		પાછા 0;
	જો (!second_edges.ગણતરી(src))
		પાછા 1;
	auto found = more_edges.find(src);
	જો (found == more_edges.end())
		પાછા 2;
	પાછા GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, પૂર્ણાંક index)
{
	જો (index == 0)
		પાછા first_edges.at(src);
	નહીંતર જો (index == 1)
		પાછા second_edges.at(src);
	નહીંતર
		પાછા *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit અચલ &bit)
{
	પસંદગી (bit.type())
	{
		સ્થિતિ DriveType::NONE:
			પાછા BitMode::NONE;
		સ્થિતિ DriveType::CONSTANT:
			// TODO how to handle Sx here?
			પાછા bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		સ્થિતિ DriveType::WIRE: {
			auto અચલ &wire = bit.wire();
			તાર્કિક driver = wire.wire->port_આગત;
			તાર્કિક driven = wire.wire->port_output;

			જો (driver && !driven)
				પાછા BitMode::DRIVER;
			નહીંતર જો (driven && !driver)
				પાછા BitMode::DRIVEN;
			નહીંતર જો (driver && driven)
				પાછા BitMode::TRISTATE;
			નહીંતર
				પાછા keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		સ્થિતિ DriveType::PORT: {
			auto અચલ &port = bit.port();
			તાર્કિક driver = celltypes.cell_output(port.cell->type, port.port);
			તાર્કિક driven = celltypes.cell_આગત(port.cell->type, port.port);
			જો (driver && !driven)
				પાછા BitMode::DRIVER;
			નહીંતર જો (driven && !driver)
				પાછા BitMode::DRIVEN_UNIQUE;
			નહીંતર
				પાછા BitMode::TRISTATE;
		}
		સ્થિતિ DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		મૂળભૂત:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit અચલ &bit)
{
	પસંદગી (bit.type())
	{
		સ્થિતિ DriveType::NONE:
			પાછા -1;
		સ્થિતિ DriveType::CONSTANT:
			પાછા (પૂર્ણાંક)bit.constant();
		સ્થિતિ DriveType::WIRE: {
			auto અચલ &wire_bit = bit.wire();
			પૂર્ણાંક offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			જો (insertion.second) {
				જો (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} નહીંતર
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			પાછા insertion.first->second.id + wire_bit.offset;
		}
		સ્થિતિ DriveType::PORT: {
			auto અચલ &port_bit = bit.port();
			auto ચાવી = std::make_pair(port_bit.cell, port_bit.port);
			પૂર્ણાંક offset = next_offset;
			auto insertion = port_offsets.emplace(ચાવી, offset);
			જો (insertion.second) {
				પૂર્ણાંક width = port_bit.cell->connections().at(port_bit.port).size();
				જો (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} નહીંતર
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			પાછા insertion.first->second.id + port_bit.offset;
		}
		મૂળભૂત:
			log_assert(ખોટું && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	જો (found_isolated != isolated_drive_bits.end())
		પાછા found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	જો (found == drive_bits.begin()) {
		પાછા id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	જો (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} નહીંતર {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	પાછા result;
}

શૂન્ય DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	જો (driven_id == driver_id)
		પાછા;

	same_driver.merge(driven_id, driver_id);

	માટે (પૂર્ણાંક i = 0, end = connected_drivers.ગણતરી(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	માટે (પૂર્ણાંક i = 0, end = connected_undirected.ગણતરી(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

શૂન્ય DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

શૂન્ય DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

શૂન્ય DriverMap::add(Module *module)
{
	માટે (auto અચલ &conn : module->connections())
		add(conn.first, conn.second);

	માટે (auto cell : module->cells())
		માટે (auto અચલ &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
શૂન્ય DriverMap::add(DriveBit અચલ &a, DriveBit અચલ &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	જો (a_id == b_id)
		પાછા;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	જો (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	નહીંતર જો (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	નહીંતર જો (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	નહીંતર જો (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	નહીંતર જો (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	નહીંતર જો (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	નહીંતર
		connect_undirected(a_id, b_id);

	પાછા;
}

// Specialized version that avoids unpacking
શૂન્ય DriverMap::add(SigSpec અચલ &a, SigSpec અચલ &b)
{
	log_assert(a.size() == b.size());
	auto અચલ &a_chunks = a.chunks();
	auto અચલ &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	પૂર્ણાંક a_offset = 0;

	auto b_chunk = b_chunks.begin();
	પૂર્ણાંક b_offset = 0;

	SigChunk tmp_a, tmp_b;
	જ્યારે (a_chunk != a_end) {
		પૂર્ણાંક a_width = a_chunk->width - a_offset;
		જો (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			ચાલુ;
		}
		પૂર્ણાંક b_width = b_chunk->width - b_offset;
		જો (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			ચાલુ;
		}
		પૂર્ણાંક width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk અચલ &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk અચલ &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

શૂન્ય DriverMap::add_port(Cell *cell, IdString port, SigSpec અચલ &b)
{
	પૂર્ણાંક offset = 0;
	માટે (auto અચલ &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

શૂન્ય DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, પૂર્ણાંક> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	માટે (પૂર્ણાંક pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		જો (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		જો (connected_drivers.contains(current))
			drivers.emplace(current);

		પૂર્ણાંક undirected_driver_ગણતરી = connected_undirected.ગણતરી(current);

		માટે (પૂર્ણાંક i = 0; i != undirected_driver_ગણતરી; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	જો (drivers.empty())
		માટે (auto seen_id : seen)
			drivers.emplace(seen_id);

	માટે (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		માટે (પૂર્ણાંક pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			પૂર્ણાંક undirected_driver_ગણતરી = connected_undirected.ગણતરી(current);

			માટે (પૂર્ણાંક i = 0; i != undirected_driver_ગણતરી; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				જો (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	માટે (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit અચલ &bit)
{
	જો (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		પાછા bit;
	જો (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		માટે (auto અચલ &inner : bit.multiple().multiple())
			result.merge((*આ)(inner));
		પાછા result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	જો (mode == BitMode::KEEP && bit_repr_id != bit_id)
		પાછા bit_repr;

	પૂર્ણાંક implicit_driver_ગણતરી = connected_drivers.ગણતરી(bit_repr_id);
	જો (connected_undirected.contains(bit_repr_id) && !oriented_present.ગણતરી(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	જો (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	માટે (પૂર્ણાંક i = 0; i != implicit_driver_ગણતરી; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	પૂર્ણાંક oriented_driver_ગણતરી = connected_oriented.ગણતરી(bit_repr_id);
	માટે (પૂર્ણાંક i = 0; i != oriented_driver_ગણતરી; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	પાછા driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	માટે (પૂર્ણાંક i = 0, width = spec.size(); i != width; ++i)
		result.append((*આ)(spec[i]));

	પાછા result;
}

std::string log_signal(DriveChunkWire અચલ &chunk)
{
	std::string id = chunk.wire->name.unescape();
	જો (chunk.is_whole())
		પાછા id;
	જો (chunk.width == 1)
		પાછા stringf("%s [%d]", id, chunk.offset);
	પાછા stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort અચલ &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	જો (chunk.is_whole())
		પાછા stringf("%s <%s>", cell_id, port_id);
	જો (chunk.width == 1)
		પાછા stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	પાછા stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker અચલ &chunk)
{
	જો (chunk.width == 1)
		પાછા stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	પાછા stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk અચલ &chunk)
{
	પસંદગી (chunk.type())
	{
		સ્થિતિ DriveType::NONE:
			પાછા stringf("<none x%d>", chunk.size());
		સ્થિતિ DriveType::CONSTANT:
			પાછા log_const(chunk.constant());
		સ્થિતિ DriveType::WIRE:
			પાછા log_signal(chunk.wire());
		સ્થિતિ DriveType::PORT:
			પાછા log_signal(chunk.port());
		સ્થિતિ DriveType::MARKER:
			પાછા log_signal(chunk.marker());
		સ્થિતિ DriveType::MULTIPLE: {
			std::string str = "<multiple";
			અચલ અક્ષર *sep = " ";
			માટે (auto અચલ &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			પાછા str;
		}
		મૂળભૂત:
			log_abort();
	}
}

std::string log_signal(DriveSpec અચલ &spec)
{
	auto &chunks = spec.chunks();
	જો (chunks.empty())
		પાછા "{}";
	જો (chunks.size() == 1)
		પાછા log_signal(chunks[0]);

	std::string str;
	અચલ અક્ષર *sep = "{ ";

	માટે (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	પાછા str;
}

YOSYS_NAMESPACE_END
