#اگر_تعریف_نشده استفادهنشده_بیتها_H
#تعریف_کن استفادهنشده_بیتها_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية استفادهنشده_بیتها
{
	SigMap sigmap;
	pool<RTLIL::Sig_بیت> استفاده_شده;

	استفادهنشده_بیتها(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_استفاده_شده(module, ct);
	}

	استفادهنشده_بیتها(RTLIL::Module *module, ثابت NewCellTypes &ct) : sigmap(module)
	{
		collect_استفاده_شده(module, ct);
	}

	فراغ collect_استفاده_شده(RTLIL::Module *module, ثابت NewCellTypes &ct)
	{
		لكل (خودکار cell : module->cells())
			لكل (خودکار &conn : cell->connections())
				إذا (!ct.cell_output(cell->type, conn.اولی))
					جمع_کن_استفاده_شده(conn.دومی);

		لكل (خودکار wire : module->wires())
			إذا (wire->پورت_output)
				جمع_کن_استفاده_شده(wire);
	}

	فراغ جمع_کن_استفاده_شده(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار بیت : sigmap(sig))
			إذا (بیت.wire != nullptr)
				استفاده_شده.درج(بیت);
	}

	bool check(RTLIL::Sig_بیت بیت) ثابت
	{
		RTLIL::Sig_بیت mapped = sigmap(بیت);
		أرجع mapped.wire != nullptr && !استفاده_شده.شمار(mapped);
	}
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
