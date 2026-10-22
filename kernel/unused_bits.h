#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/sigtools.h"
#அந்தர்பாவ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

கட்டமைப்பு UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, மாறிலி NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	வெறுமை collect_used(RTLIL::Module *module, மாறிலி NewCellTypes &ct)
	{
		க்கு (auto cell : module->cells())
			க்கு (auto &conn : cell->connections())
				எனில் (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		க்கு (auto wire : module->wires())
			எனில் (wire->port_output)
				add_used(wire);
	}

	வெறுமை add_used(மாறிலி RTLIL::SigSpec &sig)
	{
		க்கு (auto bit : sigmap(sig))
			எனில் (bit.wire != nullptr)
				used.செருகு(bit);
	}

	bool check(RTLIL::SigBit bit) மாறிலி
	{
		RTLIL::SigBit mapped = sigmap(bit);
		திரும்பு mapped.wire != nullptr && !used.மொத்தம்(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
