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

#అన్తర్భావ "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

Drive_బిట్టు::Drive_బిట్టు(Sig_బిట్టు స్థిరరాశి &బిట్టు)
{
	ఒకవేళ (బిట్టు.is_wire())
		*ఇది = Drive_బిట్టు_Wire(బిట్టు.wire, బిట్టు.స్థానభ్రంశం);
	లేకపోతే
		*ఇది = బిట్టు.దత్తాంశం;
}

శూన్య Drive_బిట్టు::విలీనం(Drive_బిట్టు స్థిరరాశి &other)
{
	ఒకవేళ (other.type_ == DriveType::NONE)
		తిరిగి;
	ఒకవేళ (type_ == DriveType::NONE) {
		*ఇది = other;
		తిరిగి;
	}
	ఒకవేళ (type_ != DriveType::గుణిజం) {
		Drive_బిట్టు_గుణిజం multi(std::move(*ఇది));
		*ఇది = std::move(multi);
	}
	గుణిజం_2().విలీనం(other);
}


శూన్య Drive_బిట్టు_గుణిజం::విలీనం(Drive_బిట్టు స్థిరరాశి &single)
{
	ఒకవేళ (single.type() == DriveType::NONE)
		తిరిగి;
	ఒకవేళ (single.type() == DriveType::గుణిజం) {
		విలీనం(single.గుణిజం_2());
		తిరిగి;
	}
	గుణిజం_3.emplace(single);
}

శూన్య Drive_బిట్టు_గుణిజం::విలీనం(Drive_బిట్టు &&single)
{
	ఒకవేళ (single.type() == DriveType::NONE)
		తిరిగి;
	ఒకవేళ (single.type() == DriveType::గుణిజం) {
		విలీనం(std::move(single.గుణిజం_2()));
		తిరిగి;
	}
	గుణిజం_3.emplace(std::move(single));
}

Drive_బిట్టు_గుణిజం Drive_Chunk_గుణిజం::క్రియాకారకం[](పూర్ణాంకం i) స్థిరరాశి
{
	Drive_బిట్టు_గుణిజం ఫలితం;
	కొరకు (స్వయంచాలక స్థిరరాశి &single : గుణిజం_3)
		ఫలితం.విలీనం(single[i]);
	తిరిగి ఫలితం;
}


తార్కిక DriveChunkWire::can_append(Drive_బిట్టు_Wire స్థిరరాశి &బిట్టు) స్థిరరాశి
{
	తిరిగి బిట్టు.wire == wire && బిట్టు.స్థానభ్రంశం == స్థానభ్రంశం + వెడల్పు;
}

తార్కిక DriveChunkWire::try_append(Drive_బిట్టు_Wire స్థిరరాశి &బిట్టు)
{
	ఒకవేళ (!can_append(బిట్టు))
		తిరిగి అబద్ధం;
	వెడల్పు += 1;
	తిరిగి నిజం;
}

తార్కిక DriveChunkWire::try_append(DriveChunkWire స్థిరరాశి &chunk)
{
	ఒకవేళ (chunk.wire != wire || chunk.స్థానభ్రంశం != స్థానభ్రంశం + వెడల్పు)
		తిరిగి అబద్ధం;
	వెడల్పు += chunk.వెడల్పు;
	తిరిగి నిజం;
}

తార్కిక Drive_Chunk_పోర్టు::can_append(Drive_బిట్టు_పోర్టు స్థిరరాశి &బిట్టు) స్థిరరాశి
{
	తిరిగి బిట్టు.cell == cell && బిట్టు.పోర్టు == పోర్టు && బిట్టు.స్థానభ్రంశం == స్థానభ్రంశం + వెడల్పు;
}

తార్కిక Drive_Chunk_పోర్టు::try_append(Drive_బిట్టు_పోర్టు స్థిరరాశి &బిట్టు)
{
	ఒకవేళ (!can_append(బిట్టు))
		తిరిగి అబద్ధం;
	వెడల్పు += 1;
	తిరిగి నిజం;
}

తార్కిక Drive_Chunk_పోర్టు::try_append(Drive_Chunk_పోర్టు స్థిరరాశి &chunk)
{
	ఒకవేళ (chunk.cell != cell || chunk.పోర్టు != పోర్టు || chunk.స్థానభ్రంశం != స్థానభ్రంశం + వెడల్పు)
		తిరిగి అబద్ధం;
	వెడల్పు += chunk.వెడల్పు;
	తిరిగి నిజం;
}

తార్కిక DriveChunkMarker::can_append(Drive_బిట్టు_Marker స్థిరరాశి &బిట్టు) స్థిరరాశి
{
	తిరిగి బిట్టు.marker == marker && బిట్టు.స్థానభ్రంశం == స్థానభ్రంశం + వెడల్పు;
}

తార్కిక DriveChunkMarker::try_append(Drive_బిట్టు_Marker స్థిరరాశి &బిట్టు)
{
	ఒకవేళ (!can_append(బిట్టు))
		తిరిగి అబద్ధం;
	వెడల్పు += 1;
	తిరిగి నిజం;
}

తార్కిక DriveChunkMarker::try_append(DriveChunkMarker స్థిరరాశి &chunk)
{
	ఒకవేళ (chunk.marker != marker || chunk.స్థానభ్రంశం != స్థానభ్రంశం + వెడల్పు)
		తిరిగి అబద్ధం;
	వెడల్పు += chunk.వెడల్పు;
	తిరిగి నిజం;
}


తార్కిక Drive_Chunk_గుణిజం::can_append(Drive_బిట్టు_గుణిజం స్థిరరాశి &బిట్టు) స్థిరరాశి
{
	ఒకవేళ (బిట్టు.గుణిజం_2().కొలత() != గుణిజం_3.కొలత())
		తిరిగి అబద్ధం;

	పూర్ణాంకం const_drivers = 0;
	కొరకు (DriveChunk స్థిరరాశి &single : గుణిజం_3)
		ఒకవేళ (single.is_constant())
			const_drivers += 1;

	ఒకవేళ (const_drivers > 1)
		తిరిగి అబద్ధం;

	కొరకు (Drive_బిట్టు స్థిరరాశి &single : బిట్టు.గుణిజం_2())
		ఒకవేళ (single.is_constant())
			const_drivers -= 1;

	ఒకవేళ (const_drivers != 0)
		తిరిగి అబద్ధం;

	కొరకు (DriveChunk స్థిరరాశి &single : గుణిజం_3)
	{
		ఎంపిక (single.type())
		{
			స్థితి DriveType::CONSTANT: {
			} ఆపు;
			స్థితి DriveType::WIRE: {
				స్వయంచాలక స్థిరరాశి &wire = single.wire();
				Drive_బిట్టు తదుపరి = Drive_బిట్టు_Wire(wire.wire, wire.స్థానభ్రంశం + wire.వెడల్పు);
				ఒకవేళ (!బిట్టు.గుణిజం_2().లెక్క(తదుపరి))
					తిరిగి అబద్ధం;
			} ఆపు;
			స్థితి DriveType::పోర్టు_2: {
				స్వయంచాలక స్థిరరాశి &పోర్టు = single.పోర్టు();
				Drive_బిట్టు తదుపరి = Drive_బిట్టు_పోర్టు(పోర్టు.cell, పోర్టు.పోర్టు, పోర్టు.స్థానభ్రంశం + పోర్టు.వెడల్పు);
				ఒకవేళ (!బిట్టు.గుణిజం_2().లెక్క(తదుపరి))
					తిరిగి అబద్ధం;
			} ఆపు;
			స్థితి DriveType::MARKER: {
				స్వయంచాలక స్థిరరాశి &marker = single.marker();
				Drive_బిట్టు తదుపరి = Drive_బిట్టు_Marker(marker.marker, marker.స్థానభ్రంశం + marker.వెడల్పు);
				ఒకవేళ (!బిట్టు.గుణిజం_2().లెక్క(తదుపరి))
					తిరిగి అబద్ధం;
			} ఆపు;
			అప్రమేయ:
				తిరిగి అబద్ధం;
		}
	}

	తిరిగి నిజం;
}

తార్కిక Drive_Chunk_గుణిజం::can_append(Drive_Chunk_గుణిజం స్థిరరాశి &chunk) స్థిరరాశి
{
	ఒకవేళ (chunk.గుణిజం_2().కొలత() != గుణిజం_3.కొలత())
		తిరిగి అబద్ధం;

	పూర్ణాంకం const_drivers = 0;
	కొరకు (DriveChunk స్థిరరాశి &single : గుణిజం_3)
		ఒకవేళ (single.is_constant())
			const_drivers += 1;

	ఒకవేళ (const_drivers > 1)
		తిరిగి అబద్ధం;

	కొరకు (DriveChunk స్థిరరాశి &single : chunk.గుణిజం_2())
		ఒకవేళ (single.is_constant())
			const_drivers -= 1;

	ఒకవేళ (const_drivers != 0)
		తిరిగి అబద్ధం;

	కొరకు (DriveChunk స్థిరరాశి &single : గుణిజం_3)
	{
		ఎంపిక (single.type())
		{
			స్థితి DriveType::CONSTANT: {
			} ఆపు;
			స్థితి DriveType::WIRE: {
				స్వయంచాలక స్థిరరాశి &wire = single.wire();
				DriveChunk తదుపరి = DriveChunkWire(wire.wire, wire.స్థానభ్రంశం + wire.వెడల్పు, chunk.కొలత());
				ఒకవేళ (!chunk.గుణిజం_2().లెక్క(తదుపరి))
					తిరిగి అబద్ధం;
			} ఆపు;
			స్థితి DriveType::పోర్టు_2: {
				స్వయంచాలక స్థిరరాశి &పోర్టు = single.పోర్టు();
				DriveChunk తదుపరి = Drive_Chunk_పోర్టు(పోర్టు.cell, పోర్టు.పోర్టు, పోర్టు.స్థానభ్రంశం + పోర్టు.వెడల్పు, chunk.కొలత());
				ఒకవేళ (!chunk.గుణిజం_2().లెక్క(తదుపరి))
					తిరిగి అబద్ధం;
			} ఆపు;
			స్థితి DriveType::MARKER: {
				స్వయంచాలక స్థిరరాశి &marker = single.marker();
				DriveChunk తదుపరి = DriveChunkMarker(marker.marker, marker.స్థానభ్రంశం + marker.వెడల్పు, chunk.కొలత());
				ఒకవేళ (!chunk.గుణిజం_2().లెక్క(తదుపరి))
					తిరిగి అబద్ధం;
			} ఆపు;
			అప్రమేయ:
				తిరిగి అబద్ధం;
		}
	}

	తిరిగి నిజం;
}

తార్కిక Drive_Chunk_గుణిజం::try_append(Drive_బిట్టు_గుణిజం స్థిరరాశి &బిట్టు)
{
	ఒకవేళ (!can_append(బిట్టు))
		తిరిగి అబద్ధం;
	వెడల్పు_3 += 1;
	State constant;

	కొరకు (Drive_బిట్టు స్థిరరాశి &single : బిట్టు.గుణిజం_2())
		ఒకవేళ (single.is_constant())
			constant = single.constant();

	కొరకు (DriveChunk &single : గుణిజం_3)
	{
		ఎంపిక (single.type())
		{
			స్థితి DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} ఆపు;
			స్థితి DriveType::WIRE: {
				single.wire().వెడల్పు += 1;
			} ఆపు;
			స్థితి DriveType::పోర్టు_2: {
				single.పోర్టు().వెడల్పు += 1;
			} ఆపు;
			స్థితి DriveType::MARKER: {
				single.marker().వెడల్పు += 1;
			} ఆపు;
			అప్రమేయ:
				log_abort();
		}
	}
	తిరిగి నిజం;
}

తార్కిక Drive_Chunk_గుణిజం::try_append(Drive_Chunk_గుణిజం స్థిరరాశి &chunk)
{
	ఒకవేళ (!can_append(chunk))
		తిరిగి అబద్ధం;
	పూర్ణాంకం వెడల్పు = chunk.కొలత();
	వెడల్పు_3 += వెడల్పు;
	Const constant;

	కొరకు (DriveChunk స్థిరరాశి &single : chunk.గుణిజం_2())
		ఒకవేళ (single.is_constant())
			constant = single.constant();

	కొరకు (DriveChunk &single : గుణిజం_3)
	{
		ఎంపిక (single.type())
		{
			స్థితి DriveType::CONSTANT: {
				single.constant().append(constant);
			} ఆపు;
			స్థితి DriveType::WIRE: {
				single.wire().వెడల్పు += వెడల్పు;
			} ఆపు;
			స్థితి DriveType::పోర్టు_2: {
				single.పోర్టు().వెడల్పు += వెడల్పు;
			} ఆపు;
			స్థితి DriveType::MARKER: {
				single.marker().వెడల్పు += వెడల్పు;
			} ఆపు;
			అప్రమేయ:
				log_abort();
		}
	}
	తిరిగి నిజం;
}

తార్కిక DriveChunk::can_append(Drive_బిట్టు స్థిరరాశి &బిట్టు) స్థిరరాశి
{
	ఒకవేళ (కొలత() == 0)
		తిరిగి నిజం;
	ఒకవేళ (బిట్టు.type() != type_)
		తిరిగి అబద్ధం;
	ఎంపిక (type_)
	{
		స్థితి DriveType::NONE:
			తిరిగి నిజం;
		స్థితి DriveType::CONSTANT:
			తిరిగి నిజం;
		స్థితి DriveType::WIRE:
			తిరిగి wire_.can_append(బిట్టు.wire());
		స్థితి DriveType::పోర్టు_2:
			తిరిగి పోర్టు_3.can_append(బిట్టు.పోర్టు());
		స్థితి DriveType::గుణిజం:
			తిరిగి గుణిజం_3.can_append(బిట్టు.గుణిజం_2());
		అప్రమేయ:
			log_abort();
	}
}

తార్కిక DriveChunk::try_append(Drive_బిట్టు స్థిరరాశి &బిట్టు)
{
	ఒకవేళ (కొలత() == 0)
		*ఇది = బిట్టు;
	ఒకవేళ (బిట్టు.type() != type_)
		తిరిగి అబద్ధం;
	ఎంపిక (type_)
	{
		స్థితి DriveType::NONE:
			none_ += 1;
			తిరిగి నిజం;
		స్థితి DriveType::CONSTANT:
			constant_.append(RTLIL::Const(బిట్టు.constant()));
			తిరిగి నిజం;
		స్థితి DriveType::WIRE:
			తిరిగి wire_.try_append(బిట్టు.wire());
		స్థితి DriveType::పోర్టు_2:
			తిరిగి పోర్టు_3.try_append(బిట్టు.పోర్టు());
		స్థితి DriveType::గుణిజం:
			తిరిగి గుణిజం_3.try_append(బిట్టు.గుణిజం_2());
		అప్రమేయ:
			log_abort();
	}
}


తార్కిక DriveChunk::try_append(DriveChunk స్థిరరాశి &chunk)
{
	ఒకవేళ (కొలత() == 0)
		*ఇది = chunk;
	ఒకవేళ (chunk.type_ != type_)
		తిరిగి అబద్ధం;
	ఎంపిక (type_)
	{
		స్థితి DriveType::NONE:
			none_ += chunk.none_;
			తిరిగి నిజం;
		స్థితి DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			తిరిగి నిజం;
		స్థితి DriveType::WIRE:
			తిరిగి wire_.try_append(chunk.wire());
		స్థితి DriveType::పోర్టు_2:
			తిరిగి పోర్టు_3.try_append(chunk.పోర్టు());
		స్థితి DriveType::MARKER:
			తిరిగి marker_.try_append(chunk.marker());
		స్థితి DriveType::గుణిజం:
			తిరిగి గుణిజం_3.try_append(chunk.గుణిజం_2());
	}
	log_abort();
}

శూన్య DriveSpec::append(Drive_బిట్టు స్థిరరాశి &బిట్టు)
{
	hash_ = 0;
	ఒకవేళ (!packed()) {
		బిట్లు_2.నెట్టు_వెనుక(బిట్టు);
		వెడల్పు_3 += 1;
		తిరిగి;
	}

	ఒకవేళ (chunks_.ఖాళీలు() || !chunks_.వెనుక().try_append(బిట్టు))
		chunks_.emplace_వెనుక(బిట్టు);
	వెడల్పు_3 += 1;
}

శూన్య DriveSpec::append(DriveChunk స్థిరరాశి &chunk)
{
	hash_ = 0;
	pack();
	ఒకవేళ (chunks_.ఖాళీలు() || !chunks_.వెనుక().try_append(chunk))
		chunks_.emplace_వెనుక(chunk);
	వెడల్పు_3 += chunk.కొలత();
}

శూన్య DriveSpec::pack() స్థిరరాశి {
	ఒకవేళ (బిట్లు_2.ఖాళీలు())
		తిరిగి;
	std::సదిశ<Drive_బిట్టు> బిట్లు(std::move(బిట్లు_2));
	కొరకు (స్వయంచాలక &బిట్టు : బిట్లు)
		ఒకవేళ (chunks_.ఖాళీలు() || !chunks_.వెనుక().try_append(బిట్టు))
			chunks_.emplace_వెనుక(std::move(బిట్టు));
}

శూన్య DriveSpec::unpack() స్థిరరాశి {
	ఒకవేళ (chunks_.ఖాళీలు())
		తిరిగి;
	కొరకు (స్వయంచాలక &chunk : chunks_)
	{
		కొరకు (పూర్ణాంకం i = 0, వెడల్పు = chunk.కొలత(); i != వెడల్పు; ++i)
		{
			బిట్లు_2.emplace_వెనుక(chunk[i]);
		}
	}
	chunks_.clear();
}

శూన్య DriveSpec::compute_వెడల్పు()
{
	వెడల్పు_3 = 0;
	కొరకు (స్వయంచాలక స్థిరరాశి &chunk : chunks_)
		వెడల్పు_3 += chunk.కొలత();
}

శూన్య DriverMap::Drive_బిట్టు_లేఖాచిత్రం::కూడు_అంచు(Drive_బిట్టు_Id src, Drive_బిట్టు_Id dst)
{
	ఒకవేళ (మొదటి_అంచులు.emplace(src, dst).మొదటి->రెండవ == dst)
		తిరిగి;
	ఒకవేళ (రెండవ_అంచులు.emplace(src, dst).మొదటి->రెండవ == dst)
		తిరిగి;
	మరిన్ని_అంచులు[src].emplace(dst);
}

DriverMap::Drive_బిట్టు_Id DriverMap::Drive_బిట్టు_లేఖాచిత్రం::తీయి_అంచు(Drive_బిట్టు_Id src)
{
	// TODO unused I think?
	స్వయంచాలక దొరికింది_మరిన్ని = మరిన్ని_అంచులు.కనుగొను(src);
	ఒకవేళ (దొరికింది_మరిన్ని != మరిన్ని_అంచులు.చివర()) {
		స్వయంచాలక ఫలితం = దొరికింది_మరిన్ని->రెండవ.తీయి();
		ఒకవేళ (దొరికింది_మరిన్ని->రెండవ.ఖాళీలు())
			మరిన్ని_అంచులు.erase(దొరికింది_మరిన్ని);
		తిరిగి ఫలితం;
	}

	స్వయంచాలక దొరికింది_రెండవ = రెండవ_అంచులు.కనుగొను(src);
	ఒకవేళ (దొరికింది_రెండవ != రెండవ_అంచులు.చివర()) {
		స్వయంచాలక ఫలితం = దొరికింది_రెండవ->రెండవ;
		రెండవ_అంచులు.erase(దొరికింది_రెండవ);
		తిరిగి ఫలితం;
	}

	స్వయంచాలక దొరికింది_మొదటి = మొదటి_అంచులు.కనుగొను(src);
	ఒకవేళ (దొరికింది_మొదటి != మొదటి_అంచులు.చివర()) {
		స్వయంచాలక ఫలితం = దొరికింది_మొదటి->రెండవ;
		మొదటి_అంచులు.erase(దొరికింది_మొదటి);
		తిరిగి ఫలితం;
	}

	తిరిగి Drive_బిట్టు_Id();
}

శూన్య DriverMap::Drive_బిట్టు_లేఖాచిత్రం::clear(Drive_బిట్టు_Id src)
{
	మొదటి_అంచులు.erase(src);
	రెండవ_అంచులు.erase(src);
	మరిన్ని_అంచులు.erase(src);
}

తార్కిక DriverMap::Drive_బిట్టు_లేఖాచిత్రం::కలిగియుంది(Drive_బిట్టు_Id src)
{
	తిరిగి మొదటి_అంచులు.లెక్క(src);
}

పూర్ణాంకం DriverMap::Drive_బిట్టు_లేఖాచిత్రం::లెక్క(Drive_బిట్టు_Id src)
{
	ఒకవేళ (!మొదటి_అంచులు.లెక్క(src))
		తిరిగి 0;
	ఒకవేళ (!రెండవ_అంచులు.లెక్క(src))
		తిరిగి 1;
	స్వయంచాలక దొరికింది = మరిన్ని_అంచులు.కనుగొను(src);
	ఒకవేళ (దొరికింది == మరిన్ని_అంచులు.చివర())
		తిరిగి 2;
	తిరిగి Get_కొలత(దొరికింది->రెండవ) + 2;
}

DriverMap::Drive_బిట్టు_Id DriverMap::Drive_బిట్టు_లేఖాచిత్రం::at(Drive_బిట్టు_Id src, పూర్ణాంకం సూచీ)
{
	ఒకవేళ (సూచీ == 0)
		తిరిగి మొదటి_అంచులు.at(src);
	లేకపోతే ఒకవేళ (సూచీ == 1)
		తిరిగి రెండవ_అంచులు.at(src);
	లేకపోతే
		తిరిగి *మరిన్ని_అంచులు.at(src).element(సూచీ - 2);
}


DriverMap::బిట్టు_బహుళకం DriverMap::బిట్టు_బహుళకం_2(Drive_బిట్టు స్థిరరాశి &బిట్టు)
{
	ఎంపిక (బిట్టు.type())
	{
		స్థితి DriveType::NONE:
			తిరిగి బిట్టు_బహుళకం::NONE;
		స్థితి DriveType::CONSTANT:
			// TODO how to handle Sx here?
			తిరిగి బిట్టు.constant() == State::Sz ? బిట్టు_బహుళకం::NONE : బిట్టు_బహుళకం::DRIVER;
		స్థితి DriveType::WIRE: {
			స్వయంచాలక స్థిరరాశి &wire = బిట్టు.wire();
			తార్కిక driver = wire.wire->పోర్టు_ఆదానం;
			తార్కిక driven = wire.wire->పోర్టు_output;

			ఒకవేళ (driver && !driven)
				తిరిగి బిట్టు_బహుళకం::DRIVER;
			లేకపోతే ఒకవేళ (driven && !driver)
				తిరిగి బిట్టు_బహుళకం::DRIVEN;
			లేకపోతే ఒకవేళ (driver && driven)
				తిరిగి బిట్టు_బహుళకం::TRISTATE;
			లేకపోతే
				తిరిగి keep_wire(బిట్టు.wire().wire) ? బిట్టు_బహుళకం::KEEP : బిట్టు_బహుళకం::NONE;
		}
		స్థితి DriveType::పోర్టు_2: {
			స్వయంచాలక స్థిరరాశి &పోర్టు = బిట్టు.పోర్టు();
			తార్కిక driver = celltypes.cell_output(పోర్టు.cell->type, పోర్టు.పోర్టు);
			తార్కిక driven = celltypes.cell_ఆదానం(పోర్టు.cell->type, పోర్టు.పోర్టు);
			ఒకవేళ (driver && !driven)
				తిరిగి బిట్టు_బహుళకం::DRIVER;
			లేకపోతే ఒకవేళ (driven && !driver)
				తిరిగి బిట్టు_బహుళకం::DRIVEN_UNIQUE;
			లేకపోతే
				తిరిగి బిట్టు_బహుళకం::TRISTATE;
		}
		స్థితి DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		అప్రమేయ:
			log_abort();
	}
}

DriverMap::Drive_బిట్టు_Id DriverMap::id_from_drive_బిట్టు(Drive_బిట్టు స్థిరరాశి &బిట్టు)
{
	ఎంపిక (బిట్టు.type())
	{
		స్థితి DriveType::NONE:
			తిరిగి -1;
		స్థితి DriveType::CONSTANT:
			తిరిగి (పూర్ణాంకం)బిట్టు.constant();
		స్థితి DriveType::WIRE: {
			స్వయంచాలక స్థిరరాశి &wire_బిట్టు = బిట్టు.wire();
			పూర్ణాంకం స్థానభ్రంశం = తదుపరి_స్థానభ్రంశం;
			స్వయంచాలక insertion = wire_offsets.emplace(wire_బిట్టు.wire, స్థానభ్రంశం);
			ఒకవేళ (insertion.రెండవ) {
				ఒకవేళ (wire_బిట్టు.wire->వెడల్పు == 1) {
					log_assert(wire_బిట్టు.స్థానభ్రంశం == 0);
					isolated_drive_బిట్లు.emplace(స్థానభ్రంశం, బిట్టు);
				} లేకపోతే
					drive_బిట్లు.emplace(స్థానభ్రంశం, Drive_బిట్టు_Wire(wire_బిట్టు.wire, 0));
				తదుపరి_స్థానభ్రంశం += wire_బిట్టు.wire->వెడల్పు;
			}
			తిరిగి insertion.మొదటి->రెండవ.id + wire_బిట్టు.స్థానభ్రంశం;
		}
		స్థితి DriveType::పోర్టు_2: {
			స్వయంచాలక స్థిరరాశి &పోర్టు_బిట్టు = బిట్టు.పోర్టు();
			స్వయంచాలక కీలకం = std::make_pair(పోర్టు_బిట్టు.cell, పోర్టు_బిట్టు.పోర్టు);
			పూర్ణాంకం స్థానభ్రంశం = తదుపరి_స్థానభ్రంశం;
			స్వయంచాలక insertion = పోర్టు_offsets.emplace(కీలకం, స్థానభ్రంశం);
			ఒకవేళ (insertion.రెండవ) {
				పూర్ణాంకం వెడల్పు = పోర్టు_బిట్టు.cell->connections().at(పోర్టు_బిట్టు.పోర్టు).కొలత();
				ఒకవేళ (వెడల్పు == 1 && స్థానభ్రంశం == 0) {
					log_assert(పోర్టు_బిట్టు.స్థానభ్రంశం == 0);
					isolated_drive_బిట్లు.emplace(స్థానభ్రంశం, బిట్టు);
				} లేకపోతే
					drive_బిట్లు.emplace(స్థానభ్రంశం, Drive_బిట్టు_పోర్టు(పోర్టు_బిట్టు.cell, పోర్టు_బిట్టు.పోర్టు, 0));
				తదుపరి_స్థానభ్రంశం += వెడల్పు;
			}
			తిరిగి insertion.మొదటి->రెండవ.id + పోర్టు_బిట్టు.స్థానభ్రంశం;
		}
		అప్రమేయ:
			log_assert(అబద్ధం && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

Drive_బిట్టు DriverMap::drive_బిట్టు_from_id(Drive_బిట్టు_Id id)
{
	స్వయంచాలక దొరికింది_isolated = isolated_drive_బిట్లు.కనుగొను(id);
	ఒకవేళ (దొరికింది_isolated != isolated_drive_బిట్లు.చివర())
		తిరిగి దొరికింది_isolated->రెండవ;

	స్వయంచాలక దొరికింది = drive_బిట్లు.పై_bound(id);
	ఒకవేళ (దొరికింది == drive_బిట్లు.begin()) {
		తిరిగి id < 0 ? Drive_బిట్టు() : Drive_బిట్టు((State) id.id);
	}
	--దొరికింది;
	Drive_బిట్టు ఫలితం = దొరికింది->రెండవ;
	ఒకవేళ (ఫలితం.is_wire()) {
		ఫలితం.wire().స్థానభ్రంశం += id.id - దొరికింది->మొదటి.id;
	} లేకపోతే {
		log_assert(ఫలితం.is_పోర్టు());
		ఫలితం.పోర్టు().స్థానభ్రంశం += id.id - దొరికింది->మొదటి.id;
	}
	తిరిగి ఫలితం;
}

శూన్య DriverMap::connect_directed_విలీనం(Drive_బిట్టు_Id driven_id, Drive_బిట్టు_Id driver_id)
{
	ఒకవేళ (driven_id == driver_id)
		తిరిగి;

	same_driver.విలీనం(driven_id, driver_id);

	కొరకు (పూర్ణాంకం i = 0, చివర = connected_drivers.లెక్క(driven_id); i != చివర; ++i)
		connected_drivers.కూడు_అంచు(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	కొరకు (పూర్ణాంకం i = 0, చివర = connected_undirected.లెక్క(driven_id); i != చివర; ++i)
		connected_undirected.కూడు_అంచు(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

శూన్య DriverMap::connect_directed_బఫరు(Drive_బిట్టు_Id driven_id, Drive_బిట్టు_Id driver_id)
{
	connected_drivers.కూడు_అంచు(driven_id, driver_id);
}

శూన్య DriverMap::connect_undirected(Drive_బిట్టు_Id a_id, Drive_బిట్టు_Id b_id)
{
	connected_undirected.కూడు_అంచు(a_id, b_id);
	connected_undirected.కూడు_అంచు(b_id, a_id);
}

శూన్య DriverMap::కూడు(Module *module)
{
	కొరకు (స్వయంచాలక స్థిరరాశి &conn : module->connections())
		కూడు(conn.మొదటి, conn.రెండవ);

	కొరకు (స్వయంచాలక cell : module->cells())
		కొరకు (స్వయంచాలక స్థిరరాశి &conn : cell->connections())
			కూడు_పోర్టు(cell, conn.మొదటి, conn.రెండవ);
}

// Add a single bit connection to the driver map.
శూన్య DriverMap::కూడు(Drive_బిట్టు స్థిరరాశి &a, Drive_బిట్టు స్థిరరాశి &b)
{
	Drive_బిట్టు_Id a_id = id_from_drive_బిట్టు(a);
	Drive_బిట్టు_Id b_id = id_from_drive_బిట్టు(b);

	Drive_బిట్టు_Id orig_a_id = a_id;
	Drive_బిట్టు_Id orig_b_id = b_id;

	a_id = same_driver.కనుగొను(a_id);
	b_id = same_driver.కనుగొను(b_id);

	ఒకవేళ (a_id == b_id)
		తిరిగి;

	బిట్టు_బహుళకం a_బహుళకం = బిట్టు_బహుళకం_2(orig_a_id == a_id ? a : drive_బిట్టు_from_id(a_id));
	బిట్టు_బహుళకం b_బహుళకం = బిట్టు_బహుళకం_2(orig_b_id == b_id ? b : drive_బిట్టు_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ఒకవేళ (a_బహుళకం == బిట్టు_బహుళకం::NONE && !(b_బహుళకం == బిట్టు_బహుళకం::DRIVEN_UNIQUE || b_బహుళకం == బిట్టు_బహుళకం::DRIVEN))
		connect_directed_విలీనం(a_id, b_id);
	లేకపోతే ఒకవేళ (b_బహుళకం == బిట్టు_బహుళకం::NONE && !(a_బహుళకం == బిట్టు_బహుళకం::DRIVEN_UNIQUE || a_బహుళకం == బిట్టు_బహుళకం::DRIVEN))
		connect_directed_విలీనం(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	లేకపోతే ఒకవేళ (a_బహుళకం == బిట్టు_బహుళకం::DRIVEN_UNIQUE && !(b_బహుళకం == బిట్టు_బహుళకం::DRIVEN_UNIQUE || b_బహుళకం == బిట్టు_బహుళకం::DRIVEN))
		connect_directed_బఫరు(a_id, b_id);
	లేకపోతే ఒకవేళ (b_బహుళకం == బిట్టు_బహుళకం::DRIVEN_UNIQUE && !(a_బహుళకం == బిట్టు_బహుళకం::DRIVEN_UNIQUE || a_బహుళకం == బిట్టు_బహుళకం::DRIVEN))
		connect_directed_బఫరు(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	లేకపోతే ఒకవేళ (a_బహుళకం == బిట్టు_బహుళకం::DRIVER)
		connect_directed_బఫరు(b_id, a_id);
	లేకపోతే ఒకవేళ (b_బహుళకం == బిట్టు_బహుళకం::DRIVER)
		connect_directed_బఫరు(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	లేకపోతే
		connect_undirected(a_id, b_id);

	తిరిగి;
}

// Specialized version that avoids unpacking
శూన్య DriverMap::కూడు(SigSpec స్థిరరాశి &a, SigSpec స్థిరరాశి &b)
{
	log_assert(a.కొలత() == b.కొలత());
	స్వయంచాలక స్థిరరాశి &a_chunks = a.chunks();
	స్వయంచాలక స్థిరరాశి &b_chunks = b.chunks();

	స్వయంచాలక a_chunk = a_chunks.begin();
	స్వయంచాలక a_చివర = a_chunks.చివర();
	పూర్ణాంకం a_స్థానభ్రంశం = 0;

	స్వయంచాలక b_chunk = b_chunks.begin();
	పూర్ణాంకం b_స్థానభ్రంశం = 0;

	SigChunk tmp_a, tmp_b;
	అయితే (a_chunk != a_చివర) {
		పూర్ణాంకం a_వెడల్పు = a_chunk->వెడల్పు - a_స్థానభ్రంశం;
		ఒకవేళ (a_వెడల్పు == 0) {
			a_స్థానభ్రంశం = 0;
			++a_chunk;
			కొనసాగు;
		}
		పూర్ణాంకం b_వెడల్పు = b_chunk->వెడల్పు - b_స్థానభ్రంశం;
		ఒకవేళ (b_వెడల్పు == 0) {
			b_స్థానభ్రంశం = 0;
			++b_chunk;
			కొనసాగు;
		}
		పూర్ణాంకం వెడల్పు = std::min(a_వెడల్పు, b_వెడల్పు);
		log_assert(వెడల్పు > 0);

		SigChunk స్థిరరాశి &a_subchunk =
			a_స్థానభ్రంశం == 0 && a_వెడల్పు == వెడల్పు ? *a_chunk : a_chunk->వెలికితీయి(a_స్థానభ్రంశం, వెడల్పు);
		SigChunk స్థిరరాశి &b_subchunk =
			b_స్థానభ్రంశం == 0 && b_వెడల్పు == వెడల్పు ? *b_chunk : b_chunk->వెలికితీయి(b_స్థానభ్రంశం, వెడల్పు);

		కూడు(a_subchunk, b_subchunk);

		a_స్థానభ్రంశం += వెడల్పు;
		b_స్థానభ్రంశం += వెడల్పు;
	}
}

శూన్య DriverMap::కూడు_పోర్టు(Cell *cell, IdString పోర్టు, SigSpec స్థిరరాశి &b)
{
	పూర్ణాంకం స్థానభ్రంశం = 0;
	కొరకు (స్వయంచాలక స్థిరరాశి &chunk : b.chunks()) {
		కూడు(chunk, Drive_Chunk_పోర్టు(cell, పోర్టు, స్థానభ్రంశం, chunk.వెడల్పు));
		స్థానభ్రంశం += chunk.కొలత();
	}
}

శూన్య DriverMap::orient_undirected(Drive_బిట్టు_Id id)
{
	pool<Drive_బిట్టు_Id> &చూసిన = orient_undirected_చూసిన;
	pool<Drive_బిట్టు_Id> &drivers = orient_undirected_drivers;
	dict<Drive_బిట్టు_Id, పూర్ణాంకం> &దూరం = orient_undirected_దూరం;
	చూసిన.clear();
	drivers.clear();

	చూసిన.emplace(id);

	కొరకు (పూర్ణాంకం pos = 0; pos < Get_కొలత(చూసిన); ++pos) {
		Drive_బిట్టు_Id current = *చూసిన.element(చూసిన.కొలత() - 1 - pos);
		Drive_బిట్టు బిట్టు = drive_బిట్టు_from_id(current);

		బిట్టు_బహుళకం బహుళకం = బిట్టు_బహుళకం_2(బిట్టు);

		ఒకవేళ (బహుళకం == బిట్టు_బహుళకం::DRIVER || బహుళకం == బిట్టు_బహుళకం::TRISTATE)
			drivers.emplace(current);

		ఒకవేళ (connected_drivers.కలిగియుంది(current))
			drivers.emplace(current);

		పూర్ణాంకం undirected_driver_లెక్క = connected_undirected.లెక్క(current);

		కొరకు (పూర్ణాంకం i = 0; i != undirected_driver_లెక్క; ++i)
			చూసిన.emplace(same_driver.కనుగొను(connected_undirected.at(current, i)));
	}

	ఒకవేళ (drivers.ఖాళీలు())
		కొరకు (స్వయంచాలక చూసిన_id : చూసిన)
			drivers.emplace(చూసిన_id);

	కొరకు (స్వయంచాలక driver : drivers)
	{
		దూరం.clear();
		దూరం.emplace(driver, 0);

		కొరకు (పూర్ణాంకం pos = 0; pos < Get_కొలత(దూరం); ++pos) {
			స్వయంచాలక current_it = దూరం.element(దూరం.కొలత() - 1 - pos);

			Drive_బిట్టు_Id current = current_it->మొదటి;
			పూర్ణాంకం undirected_driver_లెక్క = connected_undirected.లెక్క(current);

			కొరకు (పూర్ణాంకం i = 0; i != undirected_driver_లెక్క; ++i)
			{
				Drive_బిట్టు_Id తదుపరి = same_driver.కనుగొను(connected_undirected.at(current, i));
				స్వయంచాలక emplaced = దూరం.emplace(తదుపరి, current_it->రెండవ + 1);
				ఒకవేళ (emplaced.మొదటి->రెండవ == current_it->రెండవ + 1)
					connected_oriented.కూడు_అంచు(తదుపరి, current);
			}
		}
	}

	కొరకు (స్వయంచాలక చూసిన_id : చూసిన)
		oriented_present.emplace(చూసిన_id);
}

Drive_బిట్టు DriverMap::క్రియాకారకం()(Drive_బిట్టు స్థిరరాశి &బిట్టు)
{
	ఒకవేళ (బిట్టు.type() == DriveType::MARKER || బిట్టు.type() == DriveType::NONE)
		తిరిగి బిట్టు;
	ఒకవేళ (బిట్టు.type() == DriveType::గుణిజం)
	{
		Drive_బిట్టు ఫలితం;
		కొరకు (స్వయంచాలక స్థిరరాశి &inner : బిట్టు.గుణిజం_2().గుణిజం_2())
			ఫలితం.విలీనం((*ఇది)(inner));
		తిరిగి ఫలితం;
	}

	Drive_బిట్టు_Id బిట్టు_id = id_from_drive_బిట్టు(బిట్టు);

	Drive_బిట్టు_Id బిట్టు_repr_id = same_driver.కనుగొను(బిట్టు_id);

	Drive_బిట్టు బిట్టు_repr = drive_బిట్టు_from_id(బిట్టు_repr_id);

	బిట్టు_బహుళకం బహుళకం = బిట్టు_బహుళకం_2(బిట్టు_repr);

	ఒకవేళ (బహుళకం == బిట్టు_బహుళకం::KEEP && బిట్టు_repr_id != బిట్టు_id)
		తిరిగి బిట్టు_repr;

	పూర్ణాంకం implicit_driver_లెక్క = connected_drivers.లెక్క(బిట్టు_repr_id);
	ఒకవేళ (connected_undirected.కలిగియుంది(బిట్టు_repr_id) && !oriented_present.లెక్క(బిట్టు_repr_id))
		orient_undirected(బిట్టు_repr_id);

	Drive_బిట్టు driver;

	ఒకవేళ (బహుళకం == బిట్టు_బహుళకం::DRIVER || బహుళకం == బిట్టు_బహుళకం::TRISTATE)
		driver = బిట్టు_repr;

	కొరకు (పూర్ణాంకం i = 0; i != implicit_driver_లెక్క; ++i)
		driver.విలీనం(drive_బిట్టు_from_id(connected_drivers.at(బిట్టు_repr_id, i)));

	పూర్ణాంకం oriented_driver_లెక్క = connected_oriented.లెక్క(బిట్టు_repr_id);
	కొరకు (పూర్ణాంకం i = 0; i != oriented_driver_లెక్క; ++i)
		driver.విలీనం(drive_బిట్టు_from_id(connected_oriented.at(బిట్టు_repr_id, i)));

	తిరిగి driver;
}

DriveSpec DriverMap::క్రియాకారకం()(DriveSpec spec)
{
	DriveSpec ఫలితం;

	కొరకు (పూర్ణాంకం i = 0, వెడల్పు = spec.కొలత(); i != వెడల్పు; ++i)
		ఫలితం.append((*ఇది)(spec[i]));

	తిరిగి ఫలితం;
}

std::string log_signal(DriveChunkWire స్థిరరాశి &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ఒకవేళ (chunk.is_సంపూర్ణ())
		తిరిగి id;
	ఒకవేళ (chunk.వెడల్పు == 1)
		తిరిగి stringf("%s [%d]", id, chunk.స్థానభ్రంశం);
	తిరిగి stringf("%s [%d:%d]", id, chunk.స్థానభ్రంశం + chunk.వెడల్పు - 1, chunk.స్థానభ్రంశం);
}


std::string log_signal(Drive_Chunk_పోర్టు స్థిరరాశి &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string పోర్టు_id = chunk.cell->twines().unescaped_str(chunk.పోర్టు);
	ఒకవేళ (chunk.is_సంపూర్ణ())
		తిరిగి stringf("%s <%s>", cell_id, పోర్టు_id);
	ఒకవేళ (chunk.వెడల్పు == 1)
		తిరిగి stringf("%s <%s> [%d]", cell_id, పోర్టు_id, chunk.స్థానభ్రంశం);
	తిరిగి stringf("%s <%s> [%d:%d]", cell_id, పోర్టు_id, chunk.స్థానభ్రంశం + chunk.వెడల్పు - 1, chunk.స్థానభ్రంశం);
}

std::string log_signal(DriveChunkMarker స్థిరరాశి &chunk)
{
	ఒకవేళ (chunk.వెడల్పు == 1)
		తిరిగి stringf("<marker %d> [%d]", chunk.marker, chunk.స్థానభ్రంశం);
	తిరిగి stringf("<marker %d> [%d:%d]", chunk.marker, chunk.స్థానభ్రంశం + chunk.వెడల్పు - 1, chunk.స్థానభ్రంశం);
}

std::string log_signal(DriveChunk స్థిరరాశి &chunk)
{
	ఎంపిక (chunk.type())
	{
		స్థితి DriveType::NONE:
			తిరిగి stringf("<none x%d>", chunk.కొలత());
		స్థితి DriveType::CONSTANT:
			తిరిగి log_const(chunk.constant());
		స్థితి DriveType::WIRE:
			తిరిగి log_signal(chunk.wire());
		స్థితి DriveType::పోర్టు_2:
			తిరిగి log_signal(chunk.పోర్టు());
		స్థితి DriveType::MARKER:
			తిరిగి log_signal(chunk.marker());
		స్థితి DriveType::గుణిజం: {
			std::string str = "<multiple";
			స్థిరరాశి అక్షరం *sep = " ";
			కొరకు (స్వయంచాలక స్థిరరాశి &single : chunk.గుణిజం_2().గుణిజం_2()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			తిరిగి str;
		}
		అప్రమేయ:
			log_abort();
	}
}

std::string log_signal(DriveSpec స్థిరరాశి &spec)
{
	స్వయంచాలక &chunks = spec.chunks();
	ఒకవేళ (chunks.ఖాళీలు())
		తిరిగి "{}";
	ఒకవేళ (chunks.కొలత() == 1)
		తిరిగి log_signal(chunks[0]);

	std::string str;
	స్థిరరాశి అక్షరం *sep = "{ ";

	కొరకు (స్వయంచాలక i = chunks.rbegin(), చివర = chunks.rend(); i != చివర; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	తిరిగి str;
}

YOSYS_NAMESPACE_చివర
