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

#अन्तर्भाव "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit ध्रुव &bit)
{
	यदि (bit.is_wire())
		*एतत् = DriveBitWire(bit.wire, bit.विस्थापनम्);
	अन्यथा
		*एतत् = bit.दत्त;
}

शून्य DriveBit::merge(DriveBit ध्रुव &other)
{
	यदि (other.type_ == DriveType::NONE)
		प्रत्यागम;
	यदि (type_ == DriveType::NONE) {
		*एतत् = other;
		प्रत्यागम;
	}
	यदि (type_ != DriveType::गुणजः) {
		Drive_Bit_गुणजः multi(std::move(*एतत्));
		*एतत् = std::move(multi);
	}
	गुणजः_2().merge(other);
}


शून्य Drive_Bit_गुणजः::merge(DriveBit ध्रुव &single)
{
	यदि (single.type() == DriveType::NONE)
		प्रत्यागम;
	यदि (single.type() == DriveType::गुणजः) {
		merge(single.गुणजः_2());
		प्रत्यागम;
	}
	गुणजः_3.emplace(single);
}

शून्य Drive_Bit_गुणजः::merge(DriveBit &&single)
{
	यदि (single.type() == DriveType::NONE)
		प्रत्यागम;
	यदि (single.type() == DriveType::गुणजः) {
		merge(std::move(single.गुणजः_2()));
		प्रत्यागम;
	}
	गुणजः_3.emplace(std::move(single));
}

Drive_Bit_गुणजः Drive_Chunk_गुणजः::संक्रियकः[](पूर्णाङ्क i) ध्रुव
{
	Drive_Bit_गुणजः परिणाम;
	प्रति (स्वचालित ध्रुव &single : गुणजः_3)
		परिणाम.merge(single[i]);
	प्रत्यागम परिणाम;
}


तार्किक DriveChunkWire::can_append(DriveBitWire ध्रुव &bit) ध्रुव
{
	प्रत्यागम bit.wire == wire && bit.विस्थापनम् == विस्थापनम् + विस्तारः;
}

तार्किक DriveChunkWire::try_append(DriveBitWire ध्रुव &bit)
{
	यदि (!can_append(bit))
		प्रत्यागम असत्य;
	विस्तारः += 1;
	प्रत्यागम सत्य;
}

तार्किक DriveChunkWire::try_append(DriveChunkWire ध्रुव &chunk)
{
	यदि (chunk.wire != wire || chunk.विस्थापनम् != विस्थापनम् + विस्तारः)
		प्रत्यागम असत्य;
	विस्तारः += chunk.विस्तारः;
	प्रत्यागम सत्य;
}

तार्किक Drive_Chunk_पत्तनम्::can_append(Drive_Bit_पत्तनम् ध्रुव &bit) ध्रुव
{
	प्रत्यागम bit.cell == cell && bit.पत्तनम् == पत्तनम् && bit.विस्थापनम् == विस्थापनम् + विस्तारः;
}

तार्किक Drive_Chunk_पत्तनम्::try_append(Drive_Bit_पत्तनम् ध्रुव &bit)
{
	यदि (!can_append(bit))
		प्रत्यागम असत्य;
	विस्तारः += 1;
	प्रत्यागम सत्य;
}

तार्किक Drive_Chunk_पत्तनम्::try_append(Drive_Chunk_पत्तनम् ध्रुव &chunk)
{
	यदि (chunk.cell != cell || chunk.पत्तनम् != पत्तनम् || chunk.विस्थापनम् != विस्थापनम् + विस्तारः)
		प्रत्यागम असत्य;
	विस्तारः += chunk.विस्तारः;
	प्रत्यागम सत्य;
}

तार्किक DriveChunkMarker::can_append(DriveBitMarker ध्रुव &bit) ध्रुव
{
	प्रत्यागम bit.marker == marker && bit.विस्थापनम् == विस्थापनम् + विस्तारः;
}

तार्किक DriveChunkMarker::try_append(DriveBitMarker ध्रुव &bit)
{
	यदि (!can_append(bit))
		प्रत्यागम असत्य;
	विस्तारः += 1;
	प्रत्यागम सत्य;
}

तार्किक DriveChunkMarker::try_append(DriveChunkMarker ध्रुव &chunk)
{
	यदि (chunk.marker != marker || chunk.विस्थापनम् != विस्थापनम् + विस्तारः)
		प्रत्यागम असत्य;
	विस्तारः += chunk.विस्तारः;
	प्रत्यागम सत्य;
}


तार्किक Drive_Chunk_गुणजः::can_append(Drive_Bit_गुणजः ध्रुव &bit) ध्रुव
{
	यदि (bit.गुणजः_2().आकार() != गुणजः_3.आकार())
		प्रत्यागम असत्य;

	पूर्णाङ्क const_drivers = 0;
	प्रति (DriveChunk ध्रुव &single : गुणजः_3)
		यदि (single.is_constant())
			const_drivers += 1;

	यदि (const_drivers > 1)
		प्रत्यागम असत्य;

	प्रति (DriveBit ध्रुव &single : bit.गुणजः_2())
		यदि (single.is_constant())
			const_drivers -= 1;

	यदि (const_drivers != 0)
		प्रत्यागम असत्य;

	प्रति (DriveChunk ध्रुव &single : गुणजः_3)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				स्वचालित ध्रुव &wire = single.wire();
				DriveBit अग्र = DriveBitWire(wire.wire, wire.विस्थापनम् + wire.विस्तारः);
				यदि (!bit.गुणजः_2().गणनम्(अग्र))
					प्रत्यागम असत्य;
			} भङ्ग;
			पक्ष DriveType::पत्तनम्_2: {
				स्वचालित ध्रुव &पत्तनम् = single.पत्तनम्();
				DriveBit अग्र = Drive_Bit_पत्तनम्(पत्तनम्.cell, पत्तनम्.पत्तनम्, पत्तनम्.विस्थापनम् + पत्तनम्.विस्तारः);
				यदि (!bit.गुणजः_2().गणनम्(अग्र))
					प्रत्यागम असत्य;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				स्वचालित ध्रुव &marker = single.marker();
				DriveBit अग्र = DriveBitMarker(marker.marker, marker.विस्थापनम् + marker.विस्तारः);
				यदि (!bit.गुणजः_2().गणनम्(अग्र))
					प्रत्यागम असत्य;
			} भङ्ग;
			स्वभाव:
				प्रत्यागम असत्य;
		}
	}

	प्रत्यागम सत्य;
}

तार्किक Drive_Chunk_गुणजः::can_append(Drive_Chunk_गुणजः ध्रुव &chunk) ध्रुव
{
	यदि (chunk.गुणजः_2().आकार() != गुणजः_3.आकार())
		प्रत्यागम असत्य;

	पूर्णाङ्क const_drivers = 0;
	प्रति (DriveChunk ध्रुव &single : गुणजः_3)
		यदि (single.is_constant())
			const_drivers += 1;

	यदि (const_drivers > 1)
		प्रत्यागम असत्य;

	प्रति (DriveChunk ध्रुव &single : chunk.गुणजः_2())
		यदि (single.is_constant())
			const_drivers -= 1;

	यदि (const_drivers != 0)
		प्रत्यागम असत्य;

	प्रति (DriveChunk ध्रुव &single : गुणजः_3)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				स्वचालित ध्रुव &wire = single.wire();
				DriveChunk अग्र = DriveChunkWire(wire.wire, wire.विस्थापनम् + wire.विस्तारः, chunk.आकार());
				यदि (!chunk.गुणजः_2().गणनम्(अग्र))
					प्रत्यागम असत्य;
			} भङ्ग;
			पक्ष DriveType::पत्तनम्_2: {
				स्वचालित ध्रुव &पत्तनम् = single.पत्तनम्();
				DriveChunk अग्र = Drive_Chunk_पत्तनम्(पत्तनम्.cell, पत्तनम्.पत्तनम्, पत्तनम्.विस्थापनम् + पत्तनम्.विस्तारः, chunk.आकार());
				यदि (!chunk.गुणजः_2().गणनम्(अग्र))
					प्रत्यागम असत्य;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				स्वचालित ध्रुव &marker = single.marker();
				DriveChunk अग्र = DriveChunkMarker(marker.marker, marker.विस्थापनम् + marker.विस्तारः, chunk.आकार());
				यदि (!chunk.गुणजः_2().गणनम्(अग्र))
					प्रत्यागम असत्य;
			} भङ्ग;
			स्वभाव:
				प्रत्यागम असत्य;
		}
	}

	प्रत्यागम सत्य;
}

तार्किक Drive_Chunk_गुणजः::try_append(Drive_Bit_गुणजः ध्रुव &bit)
{
	यदि (!can_append(bit))
		प्रत्यागम असत्य;
	विस्तारः_3 += 1;
	State constant;

	प्रति (DriveBit ध्रुव &single : bit.गुणजः_2())
		यदि (single.is_constant())
			constant = single.constant();

	प्रति (DriveChunk &single : गुणजः_3)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				single.wire().विस्तारः += 1;
			} भङ्ग;
			पक्ष DriveType::पत्तनम्_2: {
				single.पत्तनम्().विस्तारः += 1;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				single.marker().विस्तारः += 1;
			} भङ्ग;
			स्वभाव:
				log_abort();
		}
	}
	प्रत्यागम सत्य;
}

तार्किक Drive_Chunk_गुणजः::try_append(Drive_Chunk_गुणजः ध्रुव &chunk)
{
	यदि (!can_append(chunk))
		प्रत्यागम असत्य;
	पूर्णाङ्क विस्तारः = chunk.आकार();
	विस्तारः_3 += विस्तारः;
	Const constant;

	प्रति (DriveChunk ध्रुव &single : chunk.गुणजः_2())
		यदि (single.is_constant())
			constant = single.constant();

	प्रति (DriveChunk &single : गुणजः_3)
	{
		विकल्प (single.type())
		{
			पक्ष DriveType::CONSTANT: {
				single.constant().append(constant);
			} भङ्ग;
			पक्ष DriveType::WIRE: {
				single.wire().विस्तारः += विस्तारः;
			} भङ्ग;
			पक्ष DriveType::पत्तनम्_2: {
				single.पत्तनम्().विस्तारः += विस्तारः;
			} भङ्ग;
			पक्ष DriveType::MARKER: {
				single.marker().विस्तारः += विस्तारः;
			} भङ्ग;
			स्वभाव:
				log_abort();
		}
	}
	प्रत्यागम सत्य;
}

तार्किक DriveChunk::can_append(DriveBit ध्रुव &bit) ध्रुव
{
	यदि (आकार() == 0)
		प्रत्यागम सत्य;
	यदि (bit.type() != type_)
		प्रत्यागम असत्य;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			प्रत्यागम सत्य;
		पक्ष DriveType::CONSTANT:
			प्रत्यागम सत्य;
		पक्ष DriveType::WIRE:
			प्रत्यागम wire_.can_append(bit.wire());
		पक्ष DriveType::पत्तनम्_2:
			प्रत्यागम पत्तनम्_3.can_append(bit.पत्तनम्());
		पक्ष DriveType::गुणजः:
			प्रत्यागम गुणजः_3.can_append(bit.गुणजः_2());
		स्वभाव:
			log_abort();
	}
}

तार्किक DriveChunk::try_append(DriveBit ध्रुव &bit)
{
	यदि (आकार() == 0)
		*एतत् = bit;
	यदि (bit.type() != type_)
		प्रत्यागम असत्य;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			none_ += 1;
			प्रत्यागम सत्य;
		पक्ष DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			प्रत्यागम सत्य;
		पक्ष DriveType::WIRE:
			प्रत्यागम wire_.try_append(bit.wire());
		पक्ष DriveType::पत्तनम्_2:
			प्रत्यागम पत्तनम्_3.try_append(bit.पत्तनम्());
		पक्ष DriveType::गुणजः:
			प्रत्यागम गुणजः_3.try_append(bit.गुणजः_2());
		स्वभाव:
			log_abort();
	}
}


तार्किक DriveChunk::try_append(DriveChunk ध्रुव &chunk)
{
	यदि (आकार() == 0)
		*एतत् = chunk;
	यदि (chunk.type_ != type_)
		प्रत्यागम असत्य;
	विकल्प (type_)
	{
		पक्ष DriveType::NONE:
			none_ += chunk.none_;
			प्रत्यागम सत्य;
		पक्ष DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			प्रत्यागम सत्य;
		पक्ष DriveType::WIRE:
			प्रत्यागम wire_.try_append(chunk.wire());
		पक्ष DriveType::पत्तनम्_2:
			प्रत्यागम पत्तनम्_3.try_append(chunk.पत्तनम्());
		पक्ष DriveType::MARKER:
			प्रत्यागम marker_.try_append(chunk.marker());
		पक्ष DriveType::गुणजः:
			प्रत्यागम गुणजः_3.try_append(chunk.गुणजः_2());
	}
	log_abort();
}

शून्य DriveSpec::append(DriveBit ध्रुव &bit)
{
	hash_ = 0;
	यदि (!packed()) {
		द्वयङ्काः_2.प्रक्षिप_पृष्ठभागः(bit);
		विस्तारः_3 += 1;
		प्रत्यागम;
	}

	यदि (chunks_.रिक्तानि() || !chunks_.पृष्ठभागः().try_append(bit))
		chunks_.emplace_पृष्ठभागः(bit);
	विस्तारः_3 += 1;
}

शून्य DriveSpec::append(DriveChunk ध्रुव &chunk)
{
	hash_ = 0;
	pack();
	यदि (chunks_.रिक्तानि() || !chunks_.पृष्ठभागः().try_append(chunk))
		chunks_.emplace_पृष्ठभागः(chunk);
	विस्तारः_3 += chunk.आकार();
}

शून्य DriveSpec::pack() ध्रुव {
	यदि (द्वयङ्काः_2.रिक्तानि())
		प्रत्यागम;
	std::सदिशम्<DriveBit> द्वयङ्काः(std::move(द्वयङ्काः_2));
	प्रति (स्वचालित &bit : द्वयङ्काः)
		यदि (chunks_.रिक्तानि() || !chunks_.पृष्ठभागः().try_append(bit))
			chunks_.emplace_पृष्ठभागः(std::move(bit));
}

शून्य DriveSpec::unpack() ध्रुव {
	यदि (chunks_.रिक्तानि())
		प्रत्यागम;
	प्रति (स्वचालित &chunk : chunks_)
	{
		प्रति (पूर्णाङ्क i = 0, विस्तारः = chunk.आकार(); i != विस्तारः; ++i)
		{
			द्वयङ्काः_2.emplace_पृष्ठभागः(chunk[i]);
		}
	}
	chunks_.clear();
}

शून्य DriveSpec::compute_विस्तारः()
{
	विस्तारः_3 = 0;
	प्रति (स्वचालित ध्रुव &chunk : chunks_)
		विस्तारः_3 += chunk.आकार();
}

शून्य DriverMap::DriveBitGraph::सङ्कलय_edge(DriveBitId src, DriveBitId dst)
{
	यदि (first_धाराः.emplace(src, dst).first->second == dst)
		प्रत्यागम;
	यदि (second_धाराः.emplace(src, dst).first->second == dst)
		प्रत्यागम;
	अधिकम्_धाराः[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::निष्कासय_edge(DriveBitId src)
{
	// TODO unused I think?
	स्वचालित प्राप्तम्_अधिकम् = अधिकम्_धाराः.find(src);
	यदि (प्राप्तम्_अधिकम् != अधिकम्_धाराः.अन्तः()) {
		स्वचालित परिणाम = प्राप्तम्_अधिकम्->second.निष्कासय();
		यदि (प्राप्तम्_अधिकम्->second.रिक्तानि())
			अधिकम्_धाराः.erase(प्राप्तम्_अधिकम्);
		प्रत्यागम परिणाम;
	}

	स्वचालित प्राप्तम्_second = second_धाराः.find(src);
	यदि (प्राप्तम्_second != second_धाराः.अन्तः()) {
		स्वचालित परिणाम = प्राप्तम्_second->second;
		second_धाराः.erase(प्राप्तम्_second);
		प्रत्यागम परिणाम;
	}

	स्वचालित प्राप्तम्_first = first_धाराः.find(src);
	यदि (प्राप्तम्_first != first_धाराः.अन्तः()) {
		स्वचालित परिणाम = प्राप्तम्_first->second;
		first_धाराः.erase(प्राप्तम्_first);
		प्रत्यागम परिणाम;
	}

	प्रत्यागम DriveBitId();
}

शून्य DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_धाराः.erase(src);
	second_धाराः.erase(src);
	अधिकम्_धाराः.erase(src);
}

तार्किक DriverMap::DriveBitGraph::अन्तर्भवति(DriveBitId src)
{
	प्रत्यागम first_धाराः.गणनम्(src);
}

पूर्णाङ्क DriverMap::DriveBitGraph::गणनम्(DriveBitId src)
{
	यदि (!first_धाराः.गणनम्(src))
		प्रत्यागम 0;
	यदि (!second_धाराः.गणनम्(src))
		प्रत्यागम 1;
	स्वचालित प्राप्तम् = अधिकम्_धाराः.find(src);
	यदि (प्राप्तम् == अधिकम्_धाराः.अन्तः())
		प्रत्यागम 2;
	प्रत्यागम Get_आकार(प्राप्तम्->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, पूर्णाङ्क सूचकांक)
{
	यदि (सूचकांक == 0)
		प्रत्यागम first_धाराः.at(src);
	अन्यथा यदि (सूचकांक == 1)
		प्रत्यागम second_धाराः.at(src);
	अन्यथा
		प्रत्यागम *अधिकम्_धाराः.at(src).element(सूचकांक - 2);
}


DriverMap::Bit_बहुलकम् DriverMap::bit_बहुलकम्(DriveBit ध्रुव &bit)
{
	विकल्प (bit.type())
	{
		पक्ष DriveType::NONE:
			प्रत्यागम Bit_बहुलकम्::NONE;
		पक्ष DriveType::CONSTANT:
			// TODO how to handle Sx here?
			प्रत्यागम bit.constant() == State::Sz ? Bit_बहुलकम्::NONE : Bit_बहुलकम्::DRIVER;
		पक्ष DriveType::WIRE: {
			स्वचालित ध्रुव &wire = bit.wire();
			तार्किक driver = wire.wire->पत्तनम्_निवेशः;
			तार्किक driven = wire.wire->पत्तनम्_output;

			यदि (driver && !driven)
				प्रत्यागम Bit_बहुलकम्::DRIVER;
			अन्यथा यदि (driven && !driver)
				प्रत्यागम Bit_बहुलकम्::DRIVEN;
			अन्यथा यदि (driver && driven)
				प्रत्यागम Bit_बहुलकम्::TRISTATE;
			अन्यथा
				प्रत्यागम keep_wire(bit.wire().wire) ? Bit_बहुलकम्::KEEP : Bit_बहुलकम्::NONE;
		}
		पक्ष DriveType::पत्तनम्_2: {
			स्वचालित ध्रुव &पत्तनम् = bit.पत्तनम्();
			तार्किक driver = celltypes.cell_output(पत्तनम्.cell->type, पत्तनम्.पत्तनम्);
			तार्किक driven = celltypes.cell_निवेशः(पत्तनम्.cell->type, पत्तनम्.पत्तनम्);
			यदि (driver && !driven)
				प्रत्यागम Bit_बहुलकम्::DRIVER;
			अन्यथा यदि (driven && !driver)
				प्रत्यागम Bit_बहुलकम्::DRIVEN_UNIQUE;
			अन्यथा
				प्रत्यागम Bit_बहुलकम्::TRISTATE;
		}
		पक्ष DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		स्वभाव:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit ध्रुव &bit)
{
	विकल्प (bit.type())
	{
		पक्ष DriveType::NONE:
			प्रत्यागम -1;
		पक्ष DriveType::CONSTANT:
			प्रत्यागम (पूर्णाङ्क)bit.constant();
		पक्ष DriveType::WIRE: {
			स्वचालित ध्रुव &wire_bit = bit.wire();
			पूर्णाङ्क विस्थापनम् = अग्र_विस्थापनम्;
			स्वचालित insertion = wire_offsets.emplace(wire_bit.wire, विस्थापनम्);
			यदि (insertion.second) {
				यदि (wire_bit.wire->विस्तारः == 1) {
					log_assert(wire_bit.विस्थापनम् == 0);
					isolated_drive_द्वयङ्काः.emplace(विस्थापनम्, bit);
				} अन्यथा
					drive_द्वयङ्काः.emplace(विस्थापनम्, DriveBitWire(wire_bit.wire, 0));
				अग्र_विस्थापनम् += wire_bit.wire->विस्तारः;
			}
			प्रत्यागम insertion.first->second.id + wire_bit.विस्थापनम्;
		}
		पक्ष DriveType::पत्तनम्_2: {
			स्वचालित ध्रुव &पत्तनम्_bit = bit.पत्तनम्();
			स्वचालित कुञ्चिका = std::make_pair(पत्तनम्_bit.cell, पत्तनम्_bit.पत्तनम्);
			पूर्णाङ्क विस्थापनम् = अग्र_विस्थापनम्;
			स्वचालित insertion = पत्तनम्_offsets.emplace(कुञ्चिका, विस्थापनम्);
			यदि (insertion.second) {
				पूर्णाङ्क विस्तारः = पत्तनम्_bit.cell->connections().at(पत्तनम्_bit.पत्तनम्).आकार();
				यदि (विस्तारः == 1 && विस्थापनम् == 0) {
					log_assert(पत्तनम्_bit.विस्थापनम् == 0);
					isolated_drive_द्वयङ्काः.emplace(विस्थापनम्, bit);
				} अन्यथा
					drive_द्वयङ्काः.emplace(विस्थापनम्, Drive_Bit_पत्तनम्(पत्तनम्_bit.cell, पत्तनम्_bit.पत्तनम्, 0));
				अग्र_विस्थापनम् += विस्तारः;
			}
			प्रत्यागम insertion.first->second.id + पत्तनम्_bit.विस्थापनम्;
		}
		स्वभाव:
			log_assert(असत्य && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	स्वचालित प्राप्तम्_isolated = isolated_drive_द्वयङ्काः.find(id);
	यदि (प्राप्तम्_isolated != isolated_drive_द्वयङ्काः.अन्तः())
		प्रत्यागम प्राप्तम्_isolated->second;

	स्वचालित प्राप्तम् = drive_द्वयङ्काः.ऊर्ध्वस्थम्_bound(id);
	यदि (प्राप्तम् == drive_द्वयङ्काः.begin()) {
		प्रत्यागम id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--प्राप्तम्;
	DriveBit परिणाम = प्राप्तम्->second;
	यदि (परिणाम.is_wire()) {
		परिणाम.wire().विस्थापनम् += id.id - प्राप्तम्->first.id;
	} अन्यथा {
		log_assert(परिणाम.is_पत्तनम्());
		परिणाम.पत्तनम्().विस्थापनम् += id.id - प्राप्तम्->first.id;
	}
	प्रत्यागम परिणाम;
}

शून्य DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	यदि (driven_id == driver_id)
		प्रत्यागम;

	same_driver.merge(driven_id, driver_id);

	प्रति (पूर्णाङ्क i = 0, अन्तः = connected_drivers.गणनम्(driven_id); i != अन्तः; ++i)
		connected_drivers.सङ्कलय_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	प्रति (पूर्णाङ्क i = 0, अन्तः = connected_undirected.गणनम्(driven_id); i != अन्तः; ++i)
		connected_undirected.सङ्कलय_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

शून्य DriverMap::connect_directed_बफर(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.सङ्कलय_edge(driven_id, driver_id);
}

शून्य DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.सङ्कलय_edge(a_id, b_id);
	connected_undirected.सङ्कलय_edge(b_id, a_id);
}

शून्य DriverMap::सङ्कलय(Module *module)
{
	प्रति (स्वचालित ध्रुव &conn : module->connections())
		सङ्कलय(conn.first, conn.second);

	प्रति (स्वचालित cell : module->cells())
		प्रति (स्वचालित ध्रुव &conn : cell->connections())
			सङ्कलय_पत्तनम्(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
शून्य DriverMap::सङ्कलय(DriveBit ध्रुव &a, DriveBit ध्रुव &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	यदि (a_id == b_id)
		प्रत्यागम;

	Bit_बहुलकम् a_बहुलकम् = bit_बहुलकम्(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	Bit_बहुलकम् b_बहुलकम् = bit_बहुलकम्(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	यदि (a_बहुलकम् == Bit_बहुलकम्::NONE && !(b_बहुलकम् == Bit_बहुलकम्::DRIVEN_UNIQUE || b_बहुलकम् == Bit_बहुलकम्::DRIVEN))
		connect_directed_merge(a_id, b_id);
	अन्यथा यदि (b_बहुलकम् == Bit_बहुलकम्::NONE && !(a_बहुलकम् == Bit_बहुलकम्::DRIVEN_UNIQUE || a_बहुलकम् == Bit_बहुलकम्::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	अन्यथा यदि (a_बहुलकम् == Bit_बहुलकम्::DRIVEN_UNIQUE && !(b_बहुलकम् == Bit_बहुलकम्::DRIVEN_UNIQUE || b_बहुलकम् == Bit_बहुलकम्::DRIVEN))
		connect_directed_बफर(a_id, b_id);
	अन्यथा यदि (b_बहुलकम् == Bit_बहुलकम्::DRIVEN_UNIQUE && !(a_बहुलकम् == Bit_बहुलकम्::DRIVEN_UNIQUE || a_बहुलकम् == Bit_बहुलकम्::DRIVEN))
		connect_directed_बफर(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	अन्यथा यदि (a_बहुलकम् == Bit_बहुलकम्::DRIVER)
		connect_directed_बफर(b_id, a_id);
	अन्यथा यदि (b_बहुलकम् == Bit_बहुलकम्::DRIVER)
		connect_directed_बफर(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	अन्यथा
		connect_undirected(a_id, b_id);

	प्रत्यागम;
}

// Specialized version that avoids unpacking
शून्य DriverMap::सङ्कलय(SigSpec ध्रुव &a, SigSpec ध्रुव &b)
{
	log_assert(a.आकार() == b.आकार());
	स्वचालित ध्रुव &a_chunks = a.chunks();
	स्वचालित ध्रुव &b_chunks = b.chunks();

	स्वचालित a_chunk = a_chunks.begin();
	स्वचालित a_अन्तः = a_chunks.अन्तः();
	पूर्णाङ्क a_विस्थापनम् = 0;

	स्वचालित b_chunk = b_chunks.begin();
	पूर्णाङ्क b_विस्थापनम् = 0;

	SigChunk tmp_a, tmp_b;
	यावत् (a_chunk != a_अन्तः) {
		पूर्णाङ्क a_विस्तारः = a_chunk->विस्तारः - a_विस्थापनम्;
		यदि (a_विस्तारः == 0) {
			a_विस्थापनम् = 0;
			++a_chunk;
			अनुवर्त;
		}
		पूर्णाङ्क b_विस्तारः = b_chunk->विस्तारः - b_विस्थापनम्;
		यदि (b_विस्तारः == 0) {
			b_विस्थापनम् = 0;
			++b_chunk;
			अनुवर्त;
		}
		पूर्णाङ्क विस्तारः = std::min(a_विस्तारः, b_विस्तारः);
		log_assert(विस्तारः > 0);

		SigChunk ध्रुव &a_subchunk =
			a_विस्थापनम् == 0 && a_विस्तारः == विस्तारः ? *a_chunk : a_chunk->extract(a_विस्थापनम्, विस्तारः);
		SigChunk ध्रुव &b_subchunk =
			b_विस्थापनम् == 0 && b_विस्तारः == विस्तारः ? *b_chunk : b_chunk->extract(b_विस्थापनम्, विस्तारः);

		सङ्कलय(a_subchunk, b_subchunk);

		a_विस्थापनम् += विस्तारः;
		b_विस्थापनम् += विस्तारः;
	}
}

शून्य DriverMap::सङ्कलय_पत्तनम्(Cell *cell, IdString पत्तनम्, SigSpec ध्रुव &b)
{
	पूर्णाङ्क विस्थापनम् = 0;
	प्रति (स्वचालित ध्रुव &chunk : b.chunks()) {
		सङ्कलय(chunk, Drive_Chunk_पत्तनम्(cell, पत्तनम्, विस्थापनम्, chunk.विस्तारः));
		विस्थापनम् += chunk.आकार();
	}
}

शून्य DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &आलोकितम् = orient_undirected_आलोकितम्;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, पूर्णाङ्क> &distance = orient_undirected_distance;
	आलोकितम्.clear();
	drivers.clear();

	आलोकितम्.emplace(id);

	प्रति (पूर्णाङ्क pos = 0; pos < Get_आकार(आलोकितम्); ++pos) {
		DriveBitId current = *आलोकितम्.element(आलोकितम्.आकार() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		Bit_बहुलकम् बहुलकम् = bit_बहुलकम्(bit);

		यदि (बहुलकम् == Bit_बहुलकम्::DRIVER || बहुलकम् == Bit_बहुलकम्::TRISTATE)
			drivers.emplace(current);

		यदि (connected_drivers.अन्तर्भवति(current))
			drivers.emplace(current);

		पूर्णाङ्क undirected_driver_गणनम् = connected_undirected.गणनम्(current);

		प्रति (पूर्णाङ्क i = 0; i != undirected_driver_गणनम्; ++i)
			आलोकितम्.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	यदि (drivers.रिक्तानि())
		प्रति (स्वचालित आलोकितम्_id : आलोकितम्)
			drivers.emplace(आलोकितम्_id);

	प्रति (स्वचालित driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		प्रति (पूर्णाङ्क pos = 0; pos < Get_आकार(distance); ++pos) {
			स्वचालित current_it = distance.element(distance.आकार() - 1 - pos);

			DriveBitId current = current_it->first;
			पूर्णाङ्क undirected_driver_गणनम् = connected_undirected.गणनम्(current);

			प्रति (पूर्णाङ्क i = 0; i != undirected_driver_गणनम्; ++i)
			{
				DriveBitId अग्र = same_driver.find(connected_undirected.at(current, i));
				स्वचालित emplaced = distance.emplace(अग्र, current_it->second + 1);
				यदि (emplaced.first->second == current_it->second + 1)
					connected_oriented.सङ्कलय_edge(अग्र, current);
			}
		}
	}

	प्रति (स्वचालित आलोकितम्_id : आलोकितम्)
		oriented_present.emplace(आलोकितम्_id);
}

DriveBit DriverMap::संक्रियकः()(DriveBit ध्रुव &bit)
{
	यदि (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		प्रत्यागम bit;
	यदि (bit.type() == DriveType::गुणजः)
	{
		DriveBit परिणाम;
		प्रति (स्वचालित ध्रुव &inner : bit.गुणजः_2().गुणजः_2())
			परिणाम.merge((*एतत्)(inner));
		प्रत्यागम परिणाम;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	Bit_बहुलकम् बहुलकम् = bit_बहुलकम्(bit_repr);

	यदि (बहुलकम् == Bit_बहुलकम्::KEEP && bit_repr_id != bit_id)
		प्रत्यागम bit_repr;

	पूर्णाङ्क implicit_driver_गणनम् = connected_drivers.गणनम्(bit_repr_id);
	यदि (connected_undirected.अन्तर्भवति(bit_repr_id) && !oriented_present.गणनम्(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	यदि (बहुलकम् == Bit_बहुलकम्::DRIVER || बहुलकम् == Bit_बहुलकम्::TRISTATE)
		driver = bit_repr;

	प्रति (पूर्णाङ्क i = 0; i != implicit_driver_गणनम्; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	पूर्णाङ्क oriented_driver_गणनम् = connected_oriented.गणनम्(bit_repr_id);
	प्रति (पूर्णाङ्क i = 0; i != oriented_driver_गणनम्; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	प्रत्यागम driver;
}

DriveSpec DriverMap::संक्रियकः()(DriveSpec spec)
{
	DriveSpec परिणाम;

	प्रति (पूर्णाङ्क i = 0, विस्तारः = spec.आकार(); i != विस्तारः; ++i)
		परिणाम.append((*एतत्)(spec[i]));

	प्रत्यागम परिणाम;
}

std::string log_signal(DriveChunkWire ध्रुव &chunk)
{
	std::string id = chunk.wire->name.unescape();
	यदि (chunk.is_सम्पूर्णम्())
		प्रत्यागम id;
	यदि (chunk.विस्तारः == 1)
		प्रत्यागम stringf("%s [%d]", id, chunk.विस्थापनम्);
	प्रत्यागम stringf("%s [%d:%d]", id, chunk.विस्थापनम् + chunk.विस्तारः - 1, chunk.विस्थापनम्);
}


std::string log_signal(Drive_Chunk_पत्तनम् ध्रुव &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string पत्तनम्_id = chunk.cell->twines().unescaped_str(chunk.पत्तनम्);
	यदि (chunk.is_सम्पूर्णम्())
		प्रत्यागम stringf("%s <%s>", cell_id, पत्तनम्_id);
	यदि (chunk.विस्तारः == 1)
		प्रत्यागम stringf("%s <%s> [%d]", cell_id, पत्तनम्_id, chunk.विस्थापनम्);
	प्रत्यागम stringf("%s <%s> [%d:%d]", cell_id, पत्तनम्_id, chunk.विस्थापनम् + chunk.विस्तारः - 1, chunk.विस्थापनम्);
}

std::string log_signal(DriveChunkMarker ध्रुव &chunk)
{
	यदि (chunk.विस्तारः == 1)
		प्रत्यागम stringf("<marker %d> [%d]", chunk.marker, chunk.विस्थापनम्);
	प्रत्यागम stringf("<marker %d> [%d:%d]", chunk.marker, chunk.विस्थापनम् + chunk.विस्तारः - 1, chunk.विस्थापनम्);
}

std::string log_signal(DriveChunk ध्रुव &chunk)
{
	विकल्प (chunk.type())
	{
		पक्ष DriveType::NONE:
			प्रत्यागम stringf("<none x%d>", chunk.आकार());
		पक्ष DriveType::CONSTANT:
			प्रत्यागम log_const(chunk.constant());
		पक्ष DriveType::WIRE:
			प्रत्यागम log_signal(chunk.wire());
		पक्ष DriveType::पत्तनम्_2:
			प्रत्यागम log_signal(chunk.पत्तनम्());
		पक्ष DriveType::MARKER:
			प्रत्यागम log_signal(chunk.marker());
		पक्ष DriveType::गुणजः: {
			std::string str = "<multiple";
			ध्रुव वर्ण *sep = " ";
			प्रति (स्वचालित ध्रुव &single : chunk.गुणजः_2().गुणजः_2()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			प्रत्यागम str;
		}
		स्वभाव:
			log_abort();
	}
}

std::string log_signal(DriveSpec ध्रुव &spec)
{
	स्वचालित &chunks = spec.chunks();
	यदि (chunks.रिक्तानि())
		प्रत्यागम "{}";
	यदि (chunks.आकार() == 1)
		प्रत्यागम log_signal(chunks[0]);

	std::string str;
	ध्रुव वर्ण *sep = "{ ";

	प्रति (स्वचालित i = chunks.rbegin(), अन्तः = chunks.rend(); i != अन्तः; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	प्रत्यागम str;
}

YOSYS_NAMESPACE_अन्तः
