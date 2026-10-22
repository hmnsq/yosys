#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "kernel/sigtools.h"
#অন্তর্ভাৱ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

সংরচনা UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, ধ্রুৱ NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	শূন্য collect_used(RTLIL::Module *module, ধ্রুৱ NewCellTypes &ct)
	{
		বাবে (auto cell : module->cells())
			বাবে (auto &conn : cell->connections())
				যদি (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		বাবে (auto wire : module->wires())
			যদি (wire->port_output)
				add_used(wire);
	}

	শূন্য add_used(ধ্রুৱ RTLIL::SigSpec &sig)
	{
		বাবে (auto bit : sigmap(sig))
			যদি (bit.wire != nullptr)
				used.ঢোকাও(bit);
	}

	bool check(RTLIL::SigBit bit) ধ্রুৱ
	{
		RTLIL::SigBit mapped = sigmap(bit);
		ঘূৰাই mapped.wire != nullptr && !used.গুনতি(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
