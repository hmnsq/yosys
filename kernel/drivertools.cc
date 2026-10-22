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

#ضمن "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit ثابت &bit)
{
	إذا (bit.is_wire())
		*هذا = DriveBitWire(bit.wire, bit.offset);
	وإلا
		*هذا = bit.data;
}

فراغ DriveBit::merge(DriveBit ثابت &other)
{
	إذا (other.type_ == DriveType::NONE)
		إرجاع;
	إذا (type_ == DriveType::NONE) {
		*هذا = other;
		إرجاع;
	}
	إذا (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*هذا));
		*هذا = std::move(multi);
	}
	multiple().merge(other);
}


فراغ DriveBitMultiple::merge(DriveBit ثابت &single)
{
	إذا (single.type() == DriveType::NONE)
		إرجاع;
	إذا (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		إرجاع;
	}
	multiple_.emplace(single);
}

فراغ DriveBitMultiple::merge(DriveBit &&single)
{
	إذا (single.type() == DriveType::NONE)
		إرجاع;
	إذا (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		إرجاع;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](صحيح i) ثابت
{
	DriveBitMultiple result;
	لكل (auto ثابت &single : multiple_)
		result.merge(single[i]);
	إرجاع result;
}


منطقي DriveChunkWire::can_append(DriveBitWire ثابت &bit) ثابت
{
	إرجاع bit.wire == wire && bit.offset == offset + width;
}

منطقي DriveChunkWire::try_append(DriveBitWire ثابت &bit)
{
	إذا (!can_append(bit))
		إرجاع خطأ;
	width += 1;
	إرجاع true;
}

منطقي DriveChunkWire::try_append(DriveChunkWire ثابت &chunk)
{
	إذا (chunk.wire != wire || chunk.offset != offset + width)
		إرجاع خطأ;
	width += chunk.width;
	إرجاع true;
}

منطقي DriveChunkPort::can_append(DriveBitPort ثابت &bit) ثابت
{
	إرجاع bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

منطقي DriveChunkPort::try_append(DriveBitPort ثابت &bit)
{
	إذا (!can_append(bit))
		إرجاع خطأ;
	width += 1;
	إرجاع true;
}

منطقي DriveChunkPort::try_append(DriveChunkPort ثابت &chunk)
{
	إذا (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		إرجاع خطأ;
	width += chunk.width;
	إرجاع true;
}

منطقي DriveChunkMarker::can_append(DriveBitMarker ثابت &bit) ثابت
{
	إرجاع bit.marker == marker && bit.offset == offset + width;
}

منطقي DriveChunkMarker::try_append(DriveBitMarker ثابت &bit)
{
	إذا (!can_append(bit))
		إرجاع خطأ;
	width += 1;
	إرجاع true;
}

منطقي DriveChunkMarker::try_append(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.marker != marker || chunk.offset != offset + width)
		إرجاع خطأ;
	width += chunk.width;
	إرجاع true;
}


منطقي DriveChunkMultiple::can_append(DriveBitMultiple ثابت &bit) ثابت
{
	إذا (bit.multiple().size() != multiple_.size())
		إرجاع خطأ;

	صحيح const_drivers = 0;
	لكل (DriveChunk ثابت &single : multiple_)
		إذا (single.is_constant())
			const_drivers += 1;

	إذا (const_drivers > 1)
		إرجاع خطأ;

	لكل (DriveBit ثابت &single : bit.multiple())
		إذا (single.is_constant())
			const_drivers -= 1;

	إذا (const_drivers != 0)
		إرجاع خطأ;

	لكل (DriveChunk ثابت &single : multiple_)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
			} اكسر;
			حالة DriveType::WIRE: {
				auto ثابت &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				إذا (!bit.multiple().تعداد_2(next))
					إرجاع خطأ;
			} اكسر;
			حالة DriveType::PORT: {
				auto ثابت &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				إذا (!bit.multiple().تعداد_2(next))
					إرجاع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				auto ثابت &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				إذا (!bit.multiple().تعداد_2(next))
					إرجاع خطأ;
			} اكسر;
			افتراضي:
				إرجاع خطأ;
		}
	}

	إرجاع true;
}

منطقي DriveChunkMultiple::can_append(DriveChunkMultiple ثابت &chunk) ثابت
{
	إذا (chunk.multiple().size() != multiple_.size())
		إرجاع خطأ;

	صحيح const_drivers = 0;
	لكل (DriveChunk ثابت &single : multiple_)
		إذا (single.is_constant())
			const_drivers += 1;

	إذا (const_drivers > 1)
		إرجاع خطأ;

	لكل (DriveChunk ثابت &single : chunk.multiple())
		إذا (single.is_constant())
			const_drivers -= 1;

	إذا (const_drivers != 0)
		إرجاع خطأ;

	لكل (DriveChunk ثابت &single : multiple_)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
			} اكسر;
			حالة DriveType::WIRE: {
				auto ثابت &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				إذا (!chunk.multiple().تعداد_2(next))
					إرجاع خطأ;
			} اكسر;
			حالة DriveType::PORT: {
				auto ثابت &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				إذا (!chunk.multiple().تعداد_2(next))
					إرجاع خطأ;
			} اكسر;
			حالة DriveType::MARKER: {
				auto ثابت &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				إذا (!chunk.multiple().تعداد_2(next))
					إرجاع خطأ;
			} اكسر;
			افتراضي:
				إرجاع خطأ;
		}
	}

	إرجاع true;
}

منطقي DriveChunkMultiple::try_append(DriveBitMultiple ثابت &bit)
{
	إذا (!can_append(bit))
		إرجاع خطأ;
	width_ += 1;
	State constant;

	لكل (DriveBit ثابت &single : bit.multiple())
		إذا (single.is_constant())
			constant = single.constant();

	لكل (DriveChunk &single : multiple_)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} اكسر;
			حالة DriveType::WIRE: {
				single.wire().width += 1;
			} اكسر;
			حالة DriveType::PORT: {
				single.port().width += 1;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().width += 1;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	إرجاع true;
}

منطقي DriveChunkMultiple::try_append(DriveChunkMultiple ثابت &chunk)
{
	إذا (!can_append(chunk))
		إرجاع خطأ;
	صحيح width = chunk.size();
	width_ += width;
	Const constant;

	لكل (DriveChunk ثابت &single : chunk.multiple())
		إذا (single.is_constant())
			constant = single.constant();

	لكل (DriveChunk &single : multiple_)
	{
		بدل (single.type())
		{
			حالة DriveType::CONSTANT: {
				single.constant().append(constant);
			} اكسر;
			حالة DriveType::WIRE: {
				single.wire().width += width;
			} اكسر;
			حالة DriveType::PORT: {
				single.port().width += width;
			} اكسر;
			حالة DriveType::MARKER: {
				single.marker().width += width;
			} اكسر;
			افتراضي:
				log_abort();
		}
	}
	إرجاع true;
}

منطقي DriveChunk::can_append(DriveBit ثابت &bit) ثابت
{
	إذا (size() == 0)
		إرجاع true;
	إذا (bit.type() != type_)
		إرجاع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			إرجاع true;
		حالة DriveType::CONSTANT:
			إرجاع true;
		حالة DriveType::WIRE:
			إرجاع wire_.can_append(bit.wire());
		حالة DriveType::PORT:
			إرجاع port_.can_append(bit.port());
		حالة DriveType::MULTIPLE:
			إرجاع multiple_.can_append(bit.multiple());
		افتراضي:
			log_abort();
	}
}

منطقي DriveChunk::try_append(DriveBit ثابت &bit)
{
	إذا (size() == 0)
		*هذا = bit;
	إذا (bit.type() != type_)
		إرجاع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			none_ += 1;
			إرجاع true;
		حالة DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			إرجاع true;
		حالة DriveType::WIRE:
			إرجاع wire_.try_append(bit.wire());
		حالة DriveType::PORT:
			إرجاع port_.try_append(bit.port());
		حالة DriveType::MULTIPLE:
			إرجاع multiple_.try_append(bit.multiple());
		افتراضي:
			log_abort();
	}
}


منطقي DriveChunk::try_append(DriveChunk ثابت &chunk)
{
	إذا (size() == 0)
		*هذا = chunk;
	إذا (chunk.type_ != type_)
		إرجاع خطأ;
	بدل (type_)
	{
		حالة DriveType::NONE:
			none_ += chunk.none_;
			إرجاع true;
		حالة DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			إرجاع true;
		حالة DriveType::WIRE:
			إرجاع wire_.try_append(chunk.wire());
		حالة DriveType::PORT:
			إرجاع port_.try_append(chunk.port());
		حالة DriveType::MARKER:
			إرجاع marker_.try_append(chunk.marker());
		حالة DriveType::MULTIPLE:
			إرجاع multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

فراغ DriveSpec::append(DriveBit ثابت &bit)
{
	hash_ = 0;
	إذا (!packed()) {
		bits_.ادفع_back(bit);
		width_ += 1;
		إرجاع;
	}

	إذا (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

فراغ DriveSpec::append(DriveChunk ثابت &chunk)
{
	hash_ = 0;
	pack();
	إذا (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

فراغ DriveSpec::pack() ثابت {
	إذا (bits_.empty())
		إرجاع;
	std::vector<DriveBit> bits(std::move(bits_));
	لكل (auto &bit : bits)
		إذا (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

فراغ DriveSpec::unpack() ثابت {
	إذا (chunks_.empty())
		إرجاع;
	لكل (auto &chunk : chunks_)
	{
		لكل (صحيح i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

فراغ DriveSpec::compute_width()
{
	width_ = 0;
	لكل (auto ثابت &chunk : chunks_)
		width_ += chunk.size();
}

فراغ DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	إذا (first_edges.emplace(src, dst).first->second == dst)
		إرجاع;
	إذا (second_edges.emplace(src, dst).first->second == dst)
		إرجاع;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::اسحب_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	إذا (found_more != more_edges.end()) {
		auto result = found_more->second.اسحب();
		إذا (found_more->second.empty())
			more_edges.erase(found_more);
		إرجاع result;
	}

	auto found_second = second_edges.find(src);
	إذا (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		إرجاع result;
	}

	auto found_first = first_edges.find(src);
	إذا (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		إرجاع result;
	}

	إرجاع DriveBitId();
}

فراغ DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

منطقي DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	إرجاع first_edges.تعداد_2(src);
}

صحيح DriverMap::DriveBitGraph::تعداد_2(DriveBitId src)
{
	إذا (!first_edges.تعداد_2(src))
		إرجاع 0;
	إذا (!second_edges.تعداد_2(src))
		إرجاع 1;
	auto found = more_edges.find(src);
	إذا (found == more_edges.end())
		إرجاع 2;
	إرجاع GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, صحيح index)
{
	إذا (index == 0)
		إرجاع first_edges.at(src);
	وإلا إذا (index == 1)
		إرجاع second_edges.at(src);
	وإلا
		إرجاع *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit ثابت &bit)
{
	بدل (bit.type())
	{
		حالة DriveType::NONE:
			إرجاع BitMode::NONE;
		حالة DriveType::CONSTANT:
			// TODO how to handle Sx here?
			إرجاع bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		حالة DriveType::WIRE: {
			auto ثابت &wire = bit.wire();
			منطقي driver = wire.wire->port_إدخال;
			منطقي driven = wire.wire->port_output;

			إذا (driver && !driven)
				إرجاع BitMode::DRIVER;
			وإلا إذا (driven && !driver)
				إرجاع BitMode::DRIVEN;
			وإلا إذا (driver && driven)
				إرجاع BitMode::TRISTATE;
			وإلا
				إرجاع keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		حالة DriveType::PORT: {
			auto ثابت &port = bit.port();
			منطقي driver = celltypes.cell_output(port.cell->type, port.port);
			منطقي driven = celltypes.cell_إدخال(port.cell->type, port.port);
			إذا (driver && !driven)
				إرجاع BitMode::DRIVER;
			وإلا إذا (driven && !driver)
				إرجاع BitMode::DRIVEN_UNIQUE;
			وإلا
				إرجاع BitMode::TRISTATE;
		}
		حالة DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		افتراضي:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit ثابت &bit)
{
	بدل (bit.type())
	{
		حالة DriveType::NONE:
			إرجاع -1;
		حالة DriveType::CONSTANT:
			إرجاع (صحيح)bit.constant();
		حالة DriveType::WIRE: {
			auto ثابت &wire_bit = bit.wire();
			صحيح offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			إذا (insertion.second) {
				إذا (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} وإلا
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			إرجاع insertion.first->second.id + wire_bit.offset;
		}
		حالة DriveType::PORT: {
			auto ثابت &port_bit = bit.port();
			auto مفتاح = std::make_pair(port_bit.cell, port_bit.port);
			صحيح offset = next_offset;
			auto insertion = port_offsets.emplace(مفتاح, offset);
			إذا (insertion.second) {
				صحيح width = port_bit.cell->connections().at(port_bit.port).size();
				إذا (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} وإلا
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			إرجاع insertion.first->second.id + port_bit.offset;
		}
		افتراضي:
			log_assert(خطأ && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	إذا (found_isolated != isolated_drive_bits.end())
		إرجاع found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	إذا (found == drive_bits.begin()) {
		إرجاع id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	إذا (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} وإلا {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	إرجاع result;
}

فراغ DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	إذا (driven_id == driver_id)
		إرجاع;

	same_driver.merge(driven_id, driver_id);

	لكل (صحيح i = 0, end = connected_drivers.تعداد_2(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	لكل (صحيح i = 0, end = connected_undirected.تعداد_2(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

فراغ DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

فراغ DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

فراغ DriverMap::add(Module *module)
{
	لكل (auto ثابت &conn : module->connections())
		add(conn.first, conn.second);

	لكل (auto cell : module->cells())
		لكل (auto ثابت &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
فراغ DriverMap::add(DriveBit ثابت &a, DriveBit ثابت &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	إذا (a_id == b_id)
		إرجاع;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	إذا (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	وإلا إذا (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	وإلا إذا (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	وإلا إذا (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	وإلا إذا (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	وإلا إذا (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	وإلا
		connect_undirected(a_id, b_id);

	إرجاع;
}

// Specialized version that avoids unpacking
فراغ DriverMap::add(SigSpec ثابت &a, SigSpec ثابت &b)
{
	log_assert(a.size() == b.size());
	auto ثابت &a_chunks = a.chunks();
	auto ثابت &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	صحيح a_offset = 0;

	auto b_chunk = b_chunks.begin();
	صحيح b_offset = 0;

	SigChunk tmp_a, tmp_b;
	بينما (a_chunk != a_end) {
		صحيح a_width = a_chunk->width - a_offset;
		إذا (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			استمر;
		}
		صحيح b_width = b_chunk->width - b_offset;
		إذا (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			استمر;
		}
		صحيح width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk ثابت &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk ثابت &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

فراغ DriverMap::add_port(Cell *cell, IdString port, SigSpec ثابت &b)
{
	صحيح offset = 0;
	لكل (auto ثابت &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

فراغ DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, صحيح> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	لكل (صحيح pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		إذا (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		إذا (connected_drivers.contains(current))
			drivers.emplace(current);

		صحيح undirected_driver_تعداد = connected_undirected.تعداد_2(current);

		لكل (صحيح i = 0; i != undirected_driver_تعداد; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	إذا (drivers.empty())
		لكل (auto seen_id : seen)
			drivers.emplace(seen_id);

	لكل (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		لكل (صحيح pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			صحيح undirected_driver_تعداد = connected_undirected.تعداد_2(current);

			لكل (صحيح i = 0; i != undirected_driver_تعداد; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				إذا (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	لكل (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit ثابت &bit)
{
	إذا (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		إرجاع bit;
	إذا (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		لكل (auto ثابت &inner : bit.multiple().multiple())
			result.merge((*هذا)(inner));
		إرجاع result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	إذا (mode == BitMode::KEEP && bit_repr_id != bit_id)
		إرجاع bit_repr;

	صحيح implicit_driver_تعداد = connected_drivers.تعداد_2(bit_repr_id);
	إذا (connected_undirected.contains(bit_repr_id) && !oriented_present.تعداد_2(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	إذا (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	لكل (صحيح i = 0; i != implicit_driver_تعداد; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	صحيح oriented_driver_تعداد = connected_oriented.تعداد_2(bit_repr_id);
	لكل (صحيح i = 0; i != oriented_driver_تعداد; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	إرجاع driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	لكل (صحيح i = 0, width = spec.size(); i != width; ++i)
		result.append((*هذا)(spec[i]));

	إرجاع result;
}

std::string log_signal(DriveChunkWire ثابت &chunk)
{
	std::string id = chunk.wire->name.unescape();
	إذا (chunk.is_whole())
		إرجاع id;
	إذا (chunk.width == 1)
		إرجاع stringf("%s [%d]", id, chunk.offset);
	إرجاع stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort ثابت &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	إذا (chunk.is_whole())
		إرجاع stringf("%s <%s>", cell_id, port_id);
	إذا (chunk.width == 1)
		إرجاع stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	إرجاع stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker ثابت &chunk)
{
	إذا (chunk.width == 1)
		إرجاع stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	إرجاع stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk ثابت &chunk)
{
	بدل (chunk.type())
	{
		حالة DriveType::NONE:
			إرجاع stringf("<none x%d>", chunk.size());
		حالة DriveType::CONSTANT:
			إرجاع log_const(chunk.constant());
		حالة DriveType::WIRE:
			إرجاع log_signal(chunk.wire());
		حالة DriveType::PORT:
			إرجاع log_signal(chunk.port());
		حالة DriveType::MARKER:
			إرجاع log_signal(chunk.marker());
		حالة DriveType::MULTIPLE: {
			std::string str = "<multiple";
			ثابت حرف *sep = " ";
			لكل (auto ثابت &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			إرجاع str;
		}
		افتراضي:
			log_abort();
	}
}

std::string log_signal(DriveSpec ثابت &spec)
{
	auto &chunks = spec.chunks();
	إذا (chunks.empty())
		إرجاع "{}";
	إذا (chunks.size() == 1)
		إرجاع log_signal(chunks[0]);

	std::string str;
	ثابت حرف *sep = "{ ";

	لكل (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	إرجاع str;
}

YOSYS_NAMESPACE_END
