#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/sigtools.h"
#ଅନ୍ତର୍ଭାଵ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ସଂରଚନା UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, ଅଚଳ NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	ଶୂନ୍ୟ collect_used(RTLIL::Module *module, ଅଚଳ NewCellTypes &ct)
	{
		ପାଇଁ (auto cell : module->cells())
			ପାଇଁ (auto &conn : cell->connections())
				ଯଦି (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		ପାଇଁ (auto wire : module->wires())
			ଯଦି (wire->port_output)
				add_used(wire);
	}

	ଶୂନ୍ୟ add_used(ଅଚଳ RTLIL::SigSpec &sig)
	{
		ପାଇଁ (auto bit : sigmap(sig))
			ଯଦି (bit.wire != nullptr)
				used.ଭର୍ତ୍ତି(bit);
	}

	bool check(RTLIL::SigBit bit) ଅଚଳ
	{
		RTLIL::SigBit mapped = sigmap(bit);
		ଫେରନ୍ତୁ mapped.wire != nullptr && !used.ଗଣନା_2(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
