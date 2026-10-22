#که_نه_تعریف_شوی ناکارول_شوی_بټونه_H
#تعریف_کړه ناکارول_شوی_بټونه_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية ناکارول_شوی_بټونه
{
	SigMap sigmap;
	pool<RTLIL::Sig_بټ> کارول_شوی;

	ناکارول_شوی_بټونه(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_کارول_شوی(module, ct);
	}

	ناکارول_شوی_بټونه(RTLIL::Module *module, ثابت NewCellTypes &ct) : sigmap(module)
	{
		collect_کارول_شوی(module, ct);
	}

	فراغ collect_کارول_شوی(RTLIL::Module *module, ثابت NewCellTypes &ct)
	{
		لكل (خپلکاری cell : module->cells())
			لكل (خپلکاری &conn : cell->connections())
				إذا (!ct.cell_output(cell->type, conn.لومړی))
					جمع_کړه_کارول_شوی(conn.دوهم);

		لكل (خپلکاری wire : module->wires())
			إذا (wire->پورټ_output)
				جمع_کړه_کارول_شوی(wire);
	}

	فراغ جمع_کړه_کارول_شوی(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خپلکاری بټ : sigmap(sig))
			إذا (بټ.wire != nullptr)
				کارول_شوی.ورننباسه(بټ);
	}

	bool check(RTLIL::Sig_بټ بټ) ثابت
	{
		RTLIL::Sig_بټ mapped = sigmap(بټ);
		أرجع mapped.wire != nullptr && !کارول_شوی.شمېر(mapped);
	}
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
