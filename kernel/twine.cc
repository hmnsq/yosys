#ضمن "kernel/twine.h"
#ضمن "kernel/hashcons_impl.h"
#ضمن "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

کينډۍ بنية HashConsPool<TwinePool, Twine_غوټه, IdString>;

std::ویکټور<Twine_غوټه> StaticTwines::nodes_;

فراغ StaticTwines::init() {
	إذا (ready())
		أرجع;
	log_assert(nodes_.تشه());
	nodes_.reserve(STATIC_TWINE_پای_ټکی);
	لكل (ثابت حرف *name : ID::static_نومونه)
		nodes_.emplace_شا(std::string_view(name));
}

ثابت Twine_غوټه &StaticTwines::غوټه(اندازه_t idx) { أرجع nodes_[idx]; }
منطقي StaticTwines::ready() { أرجع nodes_.اندازه() == STATIC_TWINE_پای_ټکی; }

int64_t twine_gc_ns;
صحيح twine_gc_شمېر;

std::string IdString::handle_token() ثابت {
	أرجع stringf("%s@%zu", is_عامه() ? "$pub" : "$priv", untag().raw());
}

اندازه_t IdString::handle_token_مختاړی(std::string_view token, منطقي &is_عامه_2) {
	إذا (token.substr(0, 5) == "$pub@") {
		is_عامه_2 = ریښتیا;
		أرجع 5;
	}
	إذا (token.substr(0, 6) == "$priv@") {
		is_عامه_2 = خطأ;
		أرجع 6;
	}
	أرجع 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_پای_ټکی);
	std::string پایله = ref.is_عامه() ? "\\" : "";
	پایله += static_نومونه[idx.raw()];
	أرجع پایله;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_پای_ټکی);
	أرجع static_نومونه[idx.raw()];
}

TwineSpec::TwineSpec(ونې_پاڼه v) : معلومات(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : معلومات(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : معلومات(std::move(v)) {}

فراغ SmallString::store(std::string_view content)
{
	log_assert(content.اندازه() <= MAX_LEN);
	release();
	len_ = content.اندازه();
	إذا (len_ > INLINE_CAP) {
		ptr_ = جديد حرف[len_];
		حافظه_نقل(ptr_, content.معلومات(), len_);
	} وإلا {
		حافظه_نقل(inl_, content.معلومات(), len_);
	}
}

فراغ SmallString::release()
{
	إذا (len_ > INLINE_CAP)
		ړنګول[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ثابت SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	حافظه_نقل(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::چلوونکی=(ثابت SmallString &other)
{
	إذا (هذا != &other)
		store(other.view());
	أرجع *هذا;
}

SmallString &SmallString::چلوونکی=(SmallString &&other) noexcept
{
	إذا (هذا == &other)
		أرجع *هذا;
	release();
	حافظه_نقل(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	أرجع *هذا;
}

SmallString::~SmallString() { release(); }

Twine_غوټه::Twine_غوټه(Twine_غوټه &&other) noexcept
	: متن_2(std::move(other.متن_2)), مختاړی_3(other.مختاړی_3), hash_(other.hash_)
{
	other.مختاړی_3 = DEAD;
}

Twine_غوټه &Twine_غوټه::چلوونکی=(Twine_غوټه &&other) noexcept
{
	إذا (هذا == &other)
		أرجع *هذا;
	متن_2 = std::move(other.متن_2);
	مختاړی_3 = other.مختاړی_3;
	hash_ = other.hash_;
	other.مختاړی_3 = DEAD;
	أرجع *هذا;
}

std::string TwineSpec::content_str() ثابت {
	إذا (خپلکاری *ونې_پاڼه_2 = std::get_if<ونې_پاڼه>(&معلومات))
		أرجع ونې_پاڼه_2->s;
	خپلکاری &autosfx = std::get<AutoSuffix>(معلومات);
	أرجع *autosfx.مختاړی_2 + autosfx.لکۍ;
}

ساكن std::pair<std::string_view, منطقي> unescape_name(std::string_view name) {
	إذا (!name.تشه() && name[0] == '\\')
		أرجع {name.substr(1), ریښتیا};
	أرجع {name, خطأ};
}

ساكن فراغ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	لكل (حرف ch : name)
		إذا ((بینښه)ch <= (بینښه)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(بل_serial()) {}
TwinePool::TwinePool(ثابت TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(بل_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(بل_serial()) {}

TwinePool& TwinePool::چلوونکی=(ثابت TwinePool& other) {
	إذا (هذا == &other)
		أرجع *هذا;
	HashConsPool::چلوونکی=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = بل_serial();
	أرجع *هذا;
}

TwinePool& TwinePool::چلوونکی=(TwinePool&& other) {
	إذا (هذا == &other)
		أرجع *هذا;
	HashConsPool::چلوونکی=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = بل_serial();
	أرجع *هذا;
}

منطقي TwinePool::owns(IdString ref) ثابت {
	أرجع ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	أرجع ref.stamped(serial_);
}

فراغ TwinePool::check_owned(IdString ref) ثابت {
	إذا constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ثابت Twine_غوټه& TwinePool::چلوونکی[](IdString ref) ثابت {
	check_owned(ref);
	أرجع HashConsPool::چلوونکی[](ref);
}

اندازه_t TwinePool::gc(pool<IdString>& roots) {
	لكل (خپلکاری &it : auto_prefixes)
		roots.ورننباسه(it.دوهم);
	أرجع HashConsPool::gc(roots);
}

ثابت Twine_غوټه& TwinePool::static_غوټه(اندازه_t idx) { أرجع StaticTwines::غوټه(idx); }
فراغ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

اندازه_t TwinePool::بل_serial() {
	ساكن اندازه_t counter = 0;
	اندازه_t serial = ++counter;
	إذا constexpr (IdString::MAX_SERIAL == 0)
		أرجع serial;
	وإلا
		أرجع serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::جمع_کړه_inner(Twine_غوټه t) {
	إذا (free_list.تشه() && STATIC_شمېر + backing.اندازه() > IdString::MAX_شاخص)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_شاخص - STATIC_شمېر);
	أرجع HashConsPool::جمع_کړه_inner(std::move(t));
}

فراغ TwinePool::dump(IdString ref, std::ostream& os) ثابت {
	ثابت Twine_غوټه& twine = (*هذا)[ref];
	بدل (twine.ډول()) {
	حالة Twine_غوټه::ډول_2::Dead:
		os << "Dead()";
		اكسر;
	حالة Twine_غوټه::ډول_2::ونې_پاڼه:
		os << "Leaf(\"" << twine.متن() << "\")";
		اكسر;
	حالة Twine_غوټه::ډول_2::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.مختاړی_2(), os);
		os << ", tail: \"" << twine.متن() << "\")";
		اكسر;
	}
	إذا (ref.is_عامه())
		os << " pub";
}

فراغ TwinePool::append_str(IdString ref, std::string& بهر) ثابت {
	إذا (ref == IdString::Null)
		أرجع;
	إذا (ref.is_عامه())
		بهر += '\\';
	ثابت Twine_غوټه& twine = (*هذا)[ref];
	بدل (twine.ډول()) {
	حالة Twine_غوټه::ډول_2::Dead:
		اكسر;
	حالة Twine_غوټه::ډول_2::Suffix:
		append_str(twine.مختاړی_2(), بهر);
		[[fallthrough]];
	حالة Twine_غوټه::ډول_2::ونې_پاڼه:
		بهر += twine.متن();
		اكسر;
	}
}

std::string TwinePool::str(IdString ref) ثابت {
	std::string بهر;
	append_str(ref, بهر);
	أرجع بهر;
}

std::string TwinePool::unescaped_str(IdString ref) ثابت {
	أرجع str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t مختاړی_2, std::string_view متن) ثابت {
	uint32_t hash = مختاړی_2 == Twine_غوټه::NO_مختاړی ? 0 : (*هذا)[IdString(مختاړی_2)].hash();
	أرجع Twine_غوټه::extend_hash(hash, متن);
}

IdString TwinePool::پیدا_کړه_content(uint32_t مختاړی_2, std::string_view متن) ثابت {
	أرجع پیدا_کړه_content(مختاړی_2, متن, content_hash(مختاړی_2, متن));
}

IdString TwinePool::intern(uint32_t مختاړی_2, std::string_view متن) {
	log_assert(!Multithreading::فعال());
	uint32_t hash = content_hash(مختاړی_2, متن);
	IdString ref = پیدا_کړه_content(مختاړی_2, متن, hash);
	إذا (ref != IdString::Null)
		أرجع ref;
	أرجع جمع_کړه_inner(Twine_غوټه{مختاړی_2, متن, hash});
}

IdString TwinePool::پیدا_کړه(std::string_view name) ثابت {
	إذا (name.تشه())
		أرجع IdString::Null;
	check_name(name);
	خپلکاری [content, is_عامه_2] = unescape_name(name);
	أرجع stamp(پیدا_کړه_content(Twine_غوټه::NO_مختاړی, content).نښان(is_عامه_2));
}

IdString TwinePool::پیدا_کړه(TwineSpec t) ثابت {
	إذا (std::holds_alternative<TwineSpec::AutoSuffix>(t.معلومات))
		أرجع stamp(پیدا_کړه_content(Twine_غوټه::NO_مختاړی, t.content_str()));
	إذا (خپلکاری *ونې_پاڼه_2 = std::get_if<TwineSpec::ونې_پاڼه>(&t.معلومات))
		أرجع stamp(پیدا_کړه_content(Twine_غوټه::NO_مختاړی, ونې_پاڼه_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.معلومات);
	أرجع stamp(پیدا_کړه_content((uint32_t)sfx.مختاړی_2.untag().raw(), sfx.لکۍ).نښان(sfx.مختاړی_2.is_عامه()));
}

IdString TwinePool::جمع_کړه(TwineSpec t) {
	إذا (خپلکاری *ap = std::get_if<TwineSpec::AutoSuffix>(&t.معلومات))
		أرجع جمع_کړه(auto_مختاړی(ap->مختاړی_2), ap->لکۍ);
	إذا (خپلکاری *ونې_پاڼه_2 = std::get_if<TwineSpec::ونې_پاڼه>(&t.معلومات))
		أرجع stamp(intern(Twine_غوټه::NO_مختاړی, ونې_پاڼه_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.معلومات);
	أرجع جمع_کړه(sfx.مختاړی_2, sfx.لکۍ);
}

IdString TwinePool::auto_مختاړی(ثابت std::string *مختاړی_2) {
	خپلکاری it = auto_prefixes.پیدا_کړه(مختاړی_2);
	إذا (it != auto_prefixes.پای_ټکی())
		أرجع it->دوهم;
	IdString ref = intern(Twine_غوټه::NO_مختاړی, *مختاړی_2);
	auto_prefixes[مختاړی_2] = ref;
	أرجع ref;
}

IdString TwinePool::جمع_کړه(IdString مختاړی_2, std::string_view لکۍ) {
	أرجع stamp(intern((uint32_t)مختاړی_2.untag().raw(), لکۍ).نښان(مختاړی_2.is_عامه()));
}

IdString TwinePool::جمع_کړه(std::string_view s) {
	إذا (s.تشه())
		أرجع IdString::Null;
	check_name(s);
	خپلکاری [content, is_عامه_2] = unescape_name(s);
	أرجع stamp(intern(Twine_غوټه::NO_مختاړی, content).نښان(is_عامه_2));
}

IdString TwinePool::copy_from(ثابت TwinePool& src, IdString ref) {
	IdString وموندل = پیدا_کړه_from(src, ref);
	إذا (وموندل != IdString::Null || ref == IdString::Null)
		أرجع وموندل;
	ثابت Twine_غوټه& t = src[ref.untag()];
	إذا (t.is_dead())
		أرجع IdString::Null;
	uint32_t مختاړی_2 = Twine_غوټه::NO_مختاړی;
	إذا (t.is_suffix())
		مختاړی_2 = (uint32_t)copy_from(src, t.مختاړی_2()).untag().raw();
	أرجع stamp(intern(مختاړی_2, t.متن()).نښان(ref.is_عامه()));
}

std::string TwinePool::ref_token(IdString ref) ثابت {
	أرجع "#" + std::منزل_string((uint64_t)serial_) + ":"
		+ std::منزل_string((uint64_t)stamp(ref).بټونه());
}

IdString TwinePool::ref_from_token(std::string_view token) ثابت {
	إذا (token.اندازه() < 2 || token[0] != '#')
		أرجع IdString::Null;
	اندازه_t sep = token.پیدا_کړه(':');
	إذا (sep == std::string_view::npos)
		أرجع IdString::Null;
	اندازه_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	لكل (صحيح i = 0; i < 2; i++) {
		إذا (parts[i].تشه())
			أرجع IdString::Null;
		لكل (حرف c : parts[i]) {
			إذا (c < '0' || c > '9')
				أرجع IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	إذا (fields[0] != serial_)
		أرجع IdString::Null;
	IdString ref(fields[1]);
	إذا (ref == IdString::Null)
		أرجع IdString::Null;
	أرجع is_ژوندی(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
بنية TwineSegments {
	ساكن constexpr اندازه_t SEGMENT_INLINE_ژوروالی = 8;
	TwineSegments(ثابت TwinePool &pool, IdString ref);
	TwineSegments(ثابت TwinePool &pool, IdString سر_2, std::string_view لکۍ);
	std::string_view peek();
	فراغ advance(اندازه_t n);
	اندازه_t ټول_اندازه() ثابت;
خاص:
	فراغ ټېل(std::string_view seg);
	std::string_view *segs();
	ثابت std::string_view *segs() ثابت;
	std::string_view inline_segs[SEGMENT_INLINE_ژوروالی];
	std::ویکټور<std::string_view> spill;
	اندازه_t شمېر = 0;
	اندازه_t pos = 0;
	اندازه_t off = 0;
};

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString ref)
{
	إذا (ref == IdString::Null)
		أرجع;
	لكل (IdString cur = ref.untag(); ;) {
		ثابت Twine_غوټه &t = pool[cur];
		بدل (t.ډول()) {
		حالة Twine_غوټه::ډول_2::Suffix:
			ټېل(t.متن());
			cur = t.مختاړی_2();
			استمر;
		حالة Twine_غوټه::ډول_2::ونې_پاڼه:
			ټېل(t.متن());
			اكسر;
		حالة Twine_غوټه::ډول_2::Dead:
			اكسر;
		}
		اكسر;
	}
	إذا (ref.is_عامه())
		ټېل("\\");
	std::سرچپه(segs(), segs() + شمېر);
}

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString سر_2, std::string_view لکۍ)
	: TwineSegments(pool, سر_2)
{
	ټېل(لکۍ);
}

std::string_view TwineSegments::peek()
{
	طالما (pos < شمېر) {
		std::string_view seg = segs()[pos];
		إذا (off < seg.اندازه())
			أرجع seg.substr(off);
		pos++;
		off = 0;
	}
	أرجع {};
}

فراغ TwineSegments::advance(اندازه_t n) { off += n; }

اندازه_t TwineSegments::ټول_اندازه() ثابت
{
	اندازه_t ټول = 0;
	لكل (اندازه_t i = 0; i < شمېر; i++)
		ټول += segs()[i].اندازه();
	أرجع ټول;
}

فراغ TwineSegments::ټېل(std::string_view seg)
{
	إذا (spill.تشه() && شمېر < SEGMENT_INLINE_ژوروالی) {
		inline_segs[شمېر++] = seg;
		أرجع;
	}
	إذا (spill.تشه())
		spill.ټاکل(inline_segs, inline_segs + شمېر);
	spill.ټېل_شا(seg);
	شمېر++;
}

std::string_view *TwineSegments::segs() { أرجع spill.تشه() ? inline_segs : spill.معلومات(); }
ثابت std::string_view *TwineSegments::segs() ثابت { أرجع spill.تشه() ? inline_segs : spill.معلومات(); }

اندازه_t TwinePool::str_اندازه(IdString ref) ثابت
{
	أرجع TwineSegments(*هذا, ref).ټول_اندازه();
}

منطقي TwinePool::begins_with(IdString ref, std::string_view مختاړی_2) ثابت
{
	TwineSegments segs(*هذا, ref);
	طالما (!مختاړی_2.تشه()) {
		std::string_view seg = segs.peek();
		إذا (seg.تشه())
			أرجع خطأ;
		اندازه_t n = std::min(seg.اندازه(), مختاړی_2.اندازه());
		إذا (std::memcmp(seg.معلومات(), مختاړی_2.معلومات(), n) != 0)
			أرجع خطأ;
		segs.advance(n);
		مختاړی_2.remove_مختاړی(n);
	}
	أرجع ریښتیا;
}

منطقي TwinePool::name_equal(IdString ref, std::string_view name) ثابت
{
	أرجع str_اندازه(ref) == name.اندازه() && begins_with(ref, name);
}

ساكن صحيح compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	طالما (ریښتیا) {
		std::string_view x = sa.peek(), y = sb.peek();
		إذا (x.تشه() || y.تشه())
			أرجع x.تشه() ? (y.تشه() ? 0 : -1) : 1;
		اندازه_t n = std::min(x.اندازه(), y.اندازه());
		إذا (صحيح diff = std::memcmp(x.معلومات(), y.معلومات(), n); diff != 0)
			أرجع diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::پیدا_کړه_content(uint32_t مختاړی_2, std::string_view متن, uint32_t hash) ثابت {
	أرجع پیدا_کړه_hashed(hash, [&](IdString ref) {
		ثابت Twine_غوټه &t = (*هذا)[ref];
		إذا (t == Twine_غوټه::کلی_2{مختاړی_2, متن})
			أرجع ریښتیا;
		IdString سر_2 = مختاړی_2 == Twine_غوټه::NO_مختاړی ? IdString::Null : IdString(مختاړی_2);
		TwineSegments sa(*هذا, ref), sb(*هذا, سر_2, متن);
		أرجع compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::پیدا_کړه_from(ثابت TwinePool& src, IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	IdString idx = ref.untag();
	ثابت Twine_غوټه& t = src[idx];
	إذا (t.is_dead())
		أرجع IdString::Null;
	IdString وموندل = پیدا_کړه_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*هذا, cand), sb(src, idx);
		أرجع compare_segments(sa, sb) == 0;
	});
	أرجع stamp(وموندل.نښان(ref.is_عامه()));
}

IdString TwinePool::مختاړی_د(IdString ref) ثابت {
	ثابت Twine_غوټه &t = (*هذا)[ref];
	إذا (!t.is_suffix())
		أرجع IdString::Null;
	أرجع stamp(t.مختاړی_2().نښان(ref.is_عامه()));
}

صحيح TwinePool::compare_by_name(IdString a, IdString b) ثابت
{
	إذا (a == b)
		أرجع 0;
	إذا (a == IdString::Null)
		أرجع -1;
	إذا (b == IdString::Null)
		أرجع 1;

	إذا (a.is_عامه() == b.is_عامه()) {
		ثابت Twine_غوټه &ta = (*هذا)[a.untag()];
		ثابت Twine_غوټه &tb = (*هذا)[b.untag()];
		إذا (ta.is_ونې_پاڼه() && tb.is_ونې_پاڼه())
			أرجع ta.متن().compare(tb.متن());
		إذا (ta.is_suffix() && tb.is_suffix() && ta.مختاړی_2() == tb.مختاړی_2())
			أرجع ta.متن().compare(tb.متن());
	}

	TwineSegments sa(*هذا, a), sb(*هذا, b);
	أرجع compare_segments(sa, sb);
}

YOSYS_NAMESPACE_پای_ټکی
